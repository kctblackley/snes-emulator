// Placing this here for better organisation, compared to Ricoh 5A22 CPU code

#define IMMEDIATE_DATA_TO_DIRECT_PAGE_D_READ_MODIFY_WRITE_START \
	MakeHandler(SPC700Functions::IncrementPC), \
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand0>), \
	MakeHandler(SPC700Functions::IncrementPC), \
	MakeHandler(SPC700Functions::Read<ReadFrom::PC, ReadTo::Operand>), \
	MakeHandler(SPC700Functions::IncrementPC<SubFunc::SetSubFunc>), \
	MakeHandler(SPC700Functions::Read<ReadFrom::Address, ReadTo::Operand1>),
#define IMMEDIATE_DATA_TO_DIRECT_PAGE_D_READ_MODIFY_WRITE_END \
	MakeHandler(SPC700Functions::Write<WriteValue::Operand0, WriteTo::Address>), \
	MakeHandler(SPC700Functions::NOP), \
	MakeHandler(SPC700Functions::Next)


// Addressing modes

// Namespaces
namespace ReadFrom {
	struct PC     {};
	struct StackMinus2 {};
	struct StackMinus1 {};
	struct Stack0      {};
	struct Stack1      {};
	struct Stack2      {};

	struct FFDE {};
	struct FFDF {};

	struct Address {};
}

namespace ReadTo {

	struct P  {};
	struct A  {};
	struct X  {};
	struct Y  {};

	struct PC     {};
	struct PCLow  {};
	struct PCHigh {};

	struct Operand  {};
	struct Operand0 {};
	struct Operand1 {};

	struct Discard {};
}

namespace WriteValue {
	struct P {};
	struct A {};
	struct X {};
	struct Y {};

	struct PC     {};
	struct PCLow  {};
	struct PCHigh {};

	struct Operand  {};
	struct Operand0 {};
	struct Operand1 {};
}

namespace WriteTo {
	struct StackMinus2 {};
	struct StackMinus1 {};
	struct Stack0      {};
	struct Stack1      {};
	struct Stack2      {};

	struct Address {};
}

enum class SubFunc {
	None,
	ClearISetX,
	SetNZFlagRegisterA,
	SetNZFlagRegisterX,
	SetNZFlagRegisterYA,
	SetNZFlagOperand,
	SetSubFunc,

};

// Useful code (for templating)
// if constexpr (std::is_same_v<Branch, BranchMode::N_Zero>)