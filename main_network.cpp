#include <iostream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>

int printModule(llvm::Module &module) {
    std::error_code err;
    llvm::raw_fd_ostream out("../../out/out.ll", err);

    if (!err) {
        module.print(out, nullptr);
        out.flush();
        out.close();
        if (out.has_error()) {
            std::cerr << "Could not write to file: " << err.message() << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Could not open file: " << err.message() << std::endl;
        return 2;
    }

    return 0;
}

void declFunctions(llvm::Module& module, llvm::IRBuilder<> &builder) {
    // socket
    llvm::Function::Create(
        llvm::FunctionType::get(
            builder.getInt64Ty(),
            {
                builder.getInt32Ty(),
                builder.getInt32Ty(),
                builder.getInt32Ty()
            },
            false
        ),
        llvm::Function::ExternalLinkage,
        "socket",
    module
    );

    // printf
    llvm::Function::Create(
        llvm::FunctionType::get(
            builder.getInt32Ty(),
            {
                builder.getInt8PtrTy()
            },
            true
        ),
        llvm::Function::ExternalLinkage,
        "printf",
    module
    );

    // strerror
    llvm::Function::Create(
        llvm::FunctionType::get(
            builder.getPtrTy(),
            {
                builder.getInt32Ty()
            },
            false
        ),
        llvm::Function::ExternalLinkage,
        "strerror",
    module
    );

    // _errno
    llvm::Function::Create(
        llvm::FunctionType::get(
            builder.getPtrTy(),
            {},
            false
        ),
        llvm::Function::ExternalLinkage,
        "_errno",
    module
    );

    // WSAStartup
    llvm::Function::Create(
        llvm::FunctionType::get(
            builder.getInt32Ty(),
            {
                builder.getInt16Ty(),
                builder.getPtrTy()
            },
            false
        ),
        llvm::Function::ExternalLinkage,
        "WSAStartup",
    module
    );

    // WSACleanup
    llvm::Function::Create(
        llvm::FunctionType::get(
            builder.getInt32Ty(),
            {},
            false
        ),
        llvm::Function::ExternalLinkage,
        "WSACleanup",
        module
    );
}

int main() {
    auto context = llvm::LLVMContext();
    auto module = llvm::Module("MyModule", context);
    auto builder = llvm::IRBuilder<>(context);

    declFunctions(module, builder);

    // main func
    auto mainFunc = llvm::Function::Create(
        llvm::FunctionType::get(builder.getInt32Ty(), {}, false),
        llvm::Function::ExternalLinkage,
        "main",
        module
    );
    auto mainBlock = llvm::BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(mainBlock);

    auto wsaData = llvm::StructType::create(
        context,
        {
            builder.getInt16Ty(),
            builder.getInt16Ty(),
            builder.getInt16Ty(),
            builder.getInt16Ty(),
            builder.getPtrTy(),
            llvm::ArrayType::get(builder.getInt8Ty(), 257),
            llvm::ArrayType::get(builder.getInt8Ty(), 129),
        },
    "WSAData"
    );

    auto m_wsaData = builder.CreateAlloca(wsaData, nullptr, "m_wsaData");
    auto wsaStartupResult = builder.CreateCall(
        module.getFunction("WSAStartup"),
        {
            builder.getInt16(2),
            m_wsaData
        }
    );

    // if (wsaStartupResult != 0)
    auto wsaStartupResultNotZero = builder.CreateICmpNE(wsaStartupResult, builder.getInt32(0));
    auto wsaStartupResultNotZeroBlock = llvm::BasicBlock::Create(context, "wsaStartupResultNotZero", mainFunc);
    auto wsaStartupResultZeroBlock = llvm::BasicBlock::Create(context, "wsaStartupResultZero", mainFunc);
    builder.CreateCondBr(wsaStartupResultNotZero, wsaStartupResultNotZeroBlock, wsaStartupResultZeroBlock);

    // wsaStartupResultNotZeroBlock
    builder.SetInsertPoint(wsaStartupResultNotZeroBlock);
    auto wsaStartupErrorNo = builder.CreateLoad(
        builder.getInt32Ty(),
        builder.CreateCall(
        module.getFunction("_errno"),
            {}
        )
    );
    auto wsaStartupError = builder.CreateCall(
        module.getFunction("strerror"),
        {
            wsaStartupErrorNo
        }
    );
    auto printfStr = builder.CreateGlobalStringPtr("WSAStartup failed: %s (%d)\n");
    builder.CreateCall(
        module.getFunction("printf"),
        {
            printfStr,
            wsaStartupError,
            wsaStartupErrorNo
        }
    );
    builder.CreateRet(builder.getInt32(1));

    // wsaStartupResultZeroBlock
    builder.SetInsertPoint(wsaStartupResultZeroBlock);
    auto m_socket = builder.CreateCall(
        module.getFunction("socket"),
        {
            builder.getInt32(2),
            builder.getInt32(1),
            builder.getInt32(0)
        }
    );

    // Cleanup
    builder.CreateCall(module.getFunction("WSACleanup"));
    builder.CreateRet(builder.getInt32(0));

    // Verify the module and generate IR file
    llvm::verifyFunction(*mainFunc);

    return printModule(module);
}
