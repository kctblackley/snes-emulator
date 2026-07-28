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
	struct Pointer {};
	struct PointerPlusOne {};
	struct AddressPlusOnePSW {};

	struct Address1FFF {};

	struct XPSW {}; // (X | ((PSW & 0x20) << 3))
	struct YPSW {}; // (X | ((PSW & 0x20) << 3))
}

namespace ReadTo {

	struct P  {};
	struct A  {};
	struct X  {};
	struct Y  {};

	struct PC     {};
	struct PCLow  {};
	struct PCHigh {};

	struct PointerLow  {};
	struct PointerHigh {};

	struct Operand  {};
	struct Operand0 {};
	struct Operand1 {};

	struct Operand16Low  {};
	struct Operand16High {};

	struct AddressLow  {};
	struct AddressHigh {};

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
	struct Pointer {};

	struct AddressPlusOnePSW {};

	struct Address1FFF {};

	struct XPSW {}; // (X | ((PSW & 0x20) << 3))
}

enum class SubFunc {
	None,
	ClearISetX,
	SetNZFlagRegisterA,
	SetNZFlagRegisterX,
	SetNZFlagRegisterY,	
	SetNZFlagRegisterYA,
	SetNZFlagOperand,
	SetNZFlagOperand0,
	SetSubFunc,
	IncrementAddressByX,
	IncrementAddressByY
};

enum class Bitwise {
	OR,
	AND,
	EOR,
	ADC,
	SBC,
	CMP
};

namespace Value {
	struct A {};
	struct Operand0 {};

	struct Operand  {};
	struct Operand1 {};

	struct X {};
	struct Y {};
}

// Useful code (for templating)
// if constexpr (std::is_same_v<Branch, BranchMode::N_Zero>)