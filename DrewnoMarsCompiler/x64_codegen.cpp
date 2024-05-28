#include <ostream>
#include "3ac.hpp"

namespace drewno_mars
{

	void IRProgram::allocGlobals()
	{
		// Choose a label for each global
		for (auto itr : globals)
		{
			SemSymbol *semSymbol = itr.first;
			SymOpd *symOpd = itr.second;
			std::string lbl = "gbl_" + semSymbol->getName();
			symOpd->setMemoryLoc(lbl);
		}
	}

	void IRProgram::datagenX64(std::ostream &out)
	{
		out << ".data\n";
		for (auto itr : globals) {
			if (itr.first->getName() == "console") { continue; }
			SymOpd* symOpd = itr.second;
			out << symOpd->getMemoryLoc() << ": .quad 0\n";
		}

		for (auto itr : strings) {
			LitOpd* strLbl = itr.first;
			std::string myContent = itr.second;
			out << strLbl->valString() + ": .asciz "
				<< myContent << "\n";
	}
		// Put this directive after you write out strings
		//  so that everything is aligned to a quadword value
		//  again
		out << ".align 8\n";
	}

	void IRProgram::toX64(std::ostream &out)
	{
		allocGlobals();
		datagenX64(out);
		// Iterate over each procedure and codegen it
		out << ".globl main\n";
		out << ".text\n";
		for (auto procedure : *procs)
		{
			procedure->toX64(out);
		}
	}

	void Procedure::allocLocals()
	{
		// Allocate space for locals
		// Iterate over each procedure and codegen it
		int offset = -24;
		for (auto localEntry : locals)
		{
			SymOpd *localOperand = localEntry.second;
			std::string memoryLocation = std::to_string(offset) + "(%rbp)";
			localOperand->setMemoryLoc(memoryLocation);
			offset -= int(localOperand->getWidth());
		}
		for (auto temporary : temps)
		{
			AuxOpd *auxiliaryOperand = temporary;
			std::string memoryLocation = std::to_string(offset) + "(%rbp)";
			auxiliaryOperand->setMemoryLoc(memoryLocation);
			offset -= int(auxiliaryOperand->getWidth());
		}
		for (auto formalParam : formals)
		{
			SymOpd *formalOperand = formalParam;
			std::string memoryLocation = std::to_string(offset) + "(%rbp)";
			formalOperand->setMemoryLoc(memoryLocation);
			offset -= int(formalOperand->getWidth());
		}
	}

	void Procedure::toX64(std::ostream &out)
	{
		// Allocate all locals
		allocLocals();

		enter->codegenLabels(out);
		enter->codegenX64(out);
		out << "# Fn body " << myName << "\n";
		for (auto quad : *bodyQuads)
		{
			quad->codegenLabels(out);
			out << " # " << quad->toString() << "\n";
			quad->codegenX64(out);
		}
		out << "# Fn epilogue " << myName << "\n";
		leave->codegenLabels(out);
		leave->codegenX64(out);
	}

	void Quad::codegenLabels(std::ostream &out)
	{
		if (labels.empty())
		{
			return;
		}

		size_t numLabels = labels.size();
		size_t labelIdx = 0;
		for (Label *label : labels)
		{
			out << label->getName() << ": ";
			if (labelIdx != numLabels - 1)
			{
				out << "\n";
			}
			labelIdx++;
		}
	}

	void BinOpQuad::codegenX64(std::ostream &out)
	{
		BinOp op = this->getOp();
		if (op == ADD64)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "addq "
				<< "%rbx, "
				<< "%rax\n";
			dst->genStoreVal(out, A);
		}

		else if (op == SUB64)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "subq "
				<< "%rbx, "
				<< "%rax\n";
			dst->genStoreVal(out, A);
		}
		else if (op == DIV64)
		{
			out << "cqto\n";
			out << "idivq " << RegUtils::reg64(B) << "\n";
			dst->genStoreVal(out, A);
		}
		else if (op == MULT64)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "imulq " << RegUtils::reg64(B) << "\n";
			dst->genStoreVal(out, A);
		}
		else if (op == EQ64)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "cmpq "
				<< "%rbx, "
				<< "%rax\n";
			out << "sete "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}
		else if (op == NEQ64)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "cmpq "
				<< "%rbx, "
				<< "%rax\n";
			out << "setne "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}
		else if (op == LT64)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "cmpq "
				<< "%rbx, "
				<< "%rax\n";
			out << "setl "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}
		else if (op == GT64)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "cmpq "
				<< "%rbx, "
				<< "%rax\n";
			out << "setg "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}
		else if (op == LTE64)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "cmpq "
				<< "%rbx, "
				<< "%rax\n";
			out << "setle "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}

		else if (op == GTE64)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "cmpq "
				<< "%rbx, "
				<< "%rax\n";
			out << "setge "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}

		else if (op == AND64)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "andq "
				<< "%rbx, "
				<< "%rax\n";
			dst->genStoreVal(out, A);
		}

		else if (op == OR64)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "orq "
				<< "%rax, "
				<< "%rax\n";
			dst->genStoreVal(out, A);
		}

		else if (op == ADD8)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "addb "
				<< "%bl, "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}

		else if (op == SUB8)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "subb "
				<< "%bl, "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}
		else if (op == DIV8)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "idivb "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}
		else if (op == MULT8)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "imulb "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}
		else if (op == EQ8)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "cmpq "
				<< "%bl, "
				<< "%al\n";
			out << "sete "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}
		else if (op == NEQ8)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "cmpq "
				<< "%bl, "
				<< "%al\n";
			out << "setne "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}
		else if (op == LT8)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "cmpq "
				<< "%bl, "
				<< "%al\n";
			out << "setl "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}
		else if (op == GT8)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "cmpq "
				<< "%bl, "
				<< "%al\n";
			out << "setg "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}
		else if (op == LTE8)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "cmpq "
				<< "%bl, "
				<< "%al\n";
			out << "setle "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}

		else if (op == GTE8)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "cmpq "
				<< "%bl, "
				<< "%al\n";
			out << "setge "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}

		else if (op == AND8)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "andb "
				<< "%bl, "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}

		else if (op == OR8)
		{
			src1->genLoadVal(out, A);
			src2->genLoadVal(out, B);
			out << "orb "
				<< "%bl, "
				<< "%al\n";
			dst->genStoreVal(out, A);
		}
	}

	void UnaryOpQuad::codegenX64(std::ostream &out)
	{
		src->genLoadVal(out, A);
		if (op == NOT64)
		{
			out << "cmpq $0, %rax\n"
				<< "setz %al\n";
		}
		else if (op == NEG64)
		{
			out << "negq %rax\n";
		}
		else if (op == NOT8)
		{
			out << "cmpq $0, %al\n"
				<< "setz %al\n";
		}
		else if (op == NEG8)
		{
			out << "negb %al\n";
		}
		dst->genStoreVal(out, A);
	}

	void AssignQuad::codegenX64(std::ostream &out)
	{
		src->genLoadVal(out, A);
		dst->genStoreVal(out, A);
	}

	void ReadQuad::codegenX64(std::ostream &out)
	{
		if (myDst->locString() != "console")
		{
			myDst->genLoadVal(out, DI);
		}
		else
		{
			out << "movq $1, %rdi\n";
		}
		if (myDstType->isInt())
		{
			out << "callq getInt\n";
		}
		else if (myDstType->isBool())
		{
			out << "callq getBool\n";
		}
		myDst->genStoreVal(out, A);
	}

	void MagicQuad::codegenX64(std::ostream &out)
	{
		out << "callq magic\n";
	}

	void ExitQuad::codegenX64(std::ostream &out)
	{
		out << "call exit\n";
	}

	void WriteQuad::codegenX64(std::ostream &out)
	{
		mySrc->genLoadVal(out, DI);
		if (mySrcType->isInt()) {
			out << "callq printInt\n";
		} 
		else if (mySrcType->isString()) {
			out << "callq printString\n";
		} 
		else if (mySrcType->isBool()) {
		out << "callq printBool\n";
		}
}

	void GotoQuad::codegenX64(std::ostream &out)
	{
		out << "jmp " << tgt->getName() << "\n";
	}

	void IfzQuad::codegenX64(std::ostream &out)
	{
		cnd->genLoadVal(out, DI);
		out << "cmpq $0, %rdi\n";
		out << "je " << tgt->toString();
	}

	void NopQuad::codegenX64(std::ostream &out)
	{
		out << "nop"
			<< "\n";
	}

	void CallQuad::codegenX64(std::ostream& out)
	{
		int numArgs = sym->getDataType()->asFn()->getFormalTypes()->getSize();
		if (numArgs >= 7 && numArgs % 2 != 0) {
			out << "pushq $0 \n";
	}
		out << "callq fun_" << sym->getName() << "\n";
	}

	void EnterQuad::codegenX64(std::ostream &out)
	{
		out << "pushq %rbp\n";
		out << "movq %rsp, %rbp\n";
		out << "addq $16, %rbp\n";
		out << "subq $" << myProc->arSize() << ", %rsp\n";
	}

	void LeaveQuad::codegenX64(std::ostream &out)
	{
		out << "addq $" << myProc->arSize() << ", %rsp\n";
		out << "popq %rbp\n";
		out << "retq\n";
	}

	void SetArgQuad::codegenX64(std::ostream& out) {
    // Switch based on the index value
    switch (index) { 
        case 1:
            // Load value into DI register
            opd->genLoadVal(out, DI);
            break;
        case 2:
            // Load value into SI register
            opd->genLoadVal(out, SI);
            break;
        case 3:
            // Load value into B register
            opd->genLoadVal(out, B);    
            break;
        case 4:
            // Load value into C register
            opd->genLoadVal(out, C);
            break;
        case 5:
            // Load value into D register
            opd->genLoadVal(out, D);
            break;    
        default:
            // For other indices, load value and push onto stack
            opd->genLoadVal(out, A);
            out << "pushq %rax\n";
    }
}

	void GetArgQuad::codegenX64(std::ostream& out){
	std::string memLoc = opd->getMemoryLoc();
	
	switch (index) { 
		case 1:
			out << "movq " << "%rdi, " << memLoc << "\n";
			break;
		case 2:
			out << "movq " << "%rsi, " << memLoc << "\n";
			break;
		case 3:
			out << "movq " << "%rdx, " << memLoc << "\n";
			break;
		case 4:
			out << "movq " << "%rcx, " << memLoc << "\n";
			break;
		case 5:
			out << "movq " << "%r8, " << memLoc << "\n";
			break;
		case 6:
			out << "movq " << "%r9, " << memLoc << "\n";
			break;
		default:
			size_t numArgs = myProc->getFormals().size();
			if (numArgs % 2 != 0) numArgs = numArgs + 1;
			size_t stackIndex = 8 * (numArgs - index);
			out << "movq " << stackIndex << "(%rbp), %rbx\n";
			out << "movq %rbx, " << memLoc << "\n";
			break;
	}
}

	void SetRetQuad::codegenX64(std::ostream &out)
	{
		opd->genLoadVal(out, A);
	}

	void GetRetQuad::codegenX64(std::ostream &out)
	{
		opd->genStoreVal(out, A);
	}

	void LocQuad::codegenX64(std::ostream &out)
	{
		TODO(Implement me)
	}

	void SymOpd::genLoadVal(std::ostream &out, Register reg)
	{
		out << getMovOp() << " " << getMemoryLoc() << ", " << getReg(reg) << "\n";
	}

	void SymOpd::genStoreVal(std::ostream &out, Register reg)
	{
		out << getMovOp() << " " << getReg(reg) << ", " << getMemoryLoc() << "\n";
	}

	void SymOpd::genLoadAddr(std::ostream &out, Register reg)
	{
		TODO(Implement me if necessary)
	}

	void AuxOpd::genLoadVal(std::ostream &out, Register reg)
	{
		out << getMovOp() << " " << getMemoryLoc() << ", " << getReg(reg) << "\n";
	}

	void AuxOpd::genStoreVal(std::ostream &out, Register reg)
	{
		out << getMovOp() << " " << getReg(reg) << ", " << getMemoryLoc() << "\n";
	}
	void AuxOpd::genLoadAddr(std::ostream &out, Register reg)
	{
		TODO(Implement me)
	}

	void AddrOpd::genStoreVal(std::ostream &out, Register reg)
	{
		TODO(Implement me)
	}

	void AddrOpd::genLoadVal(std::ostream &out, Register reg)
	{
		TODO(Implement me)
	}

	void AddrOpd::genStoreAddr(std::ostream &out, Register reg)
	{
		TODO(Implement me)
	}

	void AddrOpd::genLoadAddr(std::ostream &out, Register reg)
	{
		TODO(Implement me)
	}

	void LitOpd::genLoadVal(std::ostream &out, Register reg)
	{
		out << getMovOp() << " $" << val << ", " << getReg(reg) << "\n";
	}

}
