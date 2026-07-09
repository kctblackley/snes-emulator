#pragma once
#include "ricoh_5a22_operations.hpp"

constexpr Optable native_optable = {

    &nop, // 0x00

    &n_01, // 0x01

    &nop, // 0x02

    &n_03, // 0x03

    &n_04, // 0x04

    &n_05, // 0x05

    &n_06, // 0x06

    &n_07, // 0x07

    &nop, // 0x08

    &n_09, // 0x09

    &n_0a, // 0x0A

    &nop, // 0x0B

    &n_0c, // 0x0C

    &n_0d, // 0x0D

    &n_0e, // 0x0E

    &n_0f, // 0x0F

    &n_10, // 0x10

    &n_11, // 0x11

    &n_12, // 0x12

    &n_13, // 0x13

    &n_14, // 0x14

    &n_15, // 0x15

    &n_16, // 0x16

    &n_17, // 0x17

    &n_18, // 0x18

    &n_19, // 0x19

    &nop, // 0x1A

    &n_1b, // 0x1B

    &n_1c, // 0x1C

    &n_1d, // 0x1D

    &n_1e, // 0x1E

    &n_1f, // 0x1F

    &nop, // 0x20

    &n_21, // 0x21

    &nop, // 0x22

    &n_23, // 0x23

    &nop, // 0x24

    &n_25, // 0x25

    &n_26, // 0x26

    &n_27, // 0x27

    &nop, // 0x28

    &n_29, // 0x29

    &n_2a, // 0x2A

    &nop, // 0x2B

    &nop, // 0x2C

    &n_2d, // 0x2D

    &n_2e, // 0x2E

    &n_2f, // 0x2F

    &n_30, // 0x30

    &n_31, // 0x31

    &n_32, // 0x32

    &n_33, // 0x33

    &nop, // 0x34

    &n_35, // 0x35

    &nop, // 0x36

    &n_37, // 0x37

    &n_38, // 0x38

    &n_39, // 0x39

    &nop, // 0x3A

    &n_3b, // 0x3B

    &nop, // 0x3C

    &n_3d, // 0x3D

    &nop, // 0x3E

    &n_3f, // 0x3F

    &nop, // 0x40

    &n_41, // 0x41

    &nop, // 0x42

    &n_43, // 0x43

    &n_44, // 0x44

    &n_45, // 0x45

    &n_46, // 0x46

    &n_47, // 0x47

    &nop, // 0x48

    &n_49, // 0x49

    &n_4a, // 0x4A

    &nop, // 0x4B

    &nop, // 0x4C

    &n_4d, // 0x4D

    &n_4e, // 0x4E

    &n_4f, // 0x4F

    &n_50, // 0x50

    &n_51, // 0x51

    &n_52, // 0x52

    &n_53, // 0x53

    &n_54, // 0x54

    &n_55, // 0x55

    &n_56, // 0x56

    &n_57, // 0x57

    &n_58, // 0x58

    &n_59, // 0x59

    &nop, // 0x5A

    &n_5b, // 0x5B

    &nop, // 0x5C

    &n_5d, // 0x5D

    &n_5e, // 0x5E

    &n_5f, // 0x5F

    &nop, // 0x60

    &n_61, // 0x61

    &nop, // 0x62

    &n_63, // 0x63

    &nop, // 0x64

    &n_65, // 0x65

    &n_66, // 0x66

    &n_67, // 0x67

    &nop, // 0x68

    &n_69, // 0x69

    &n_6a, // 0x6A

    &nop, // 0x6B

    &nop, // 0x6C

    &n_6d, // 0x6D

    &n_6e, // 0x6E

    &n_6f, // 0x6F

    &n_70, // 0x70

    &n_71, // 0x71

    &n_72, // 0x72

    &n_73, // 0x73

    &nop, // 0x74

    &n_75, // 0x75

    &n_76, // 0x76

    &n_77, // 0x77

    &n_78, // 0x78

    &n_79, // 0x79

    &nop, // 0x7A

    &n_7b, // 0x7B

    &nop, // 0x7C

    &n_7d, // 0x7D

    &n_7e, // 0x7E

    &n_7f, // 0x7F

    &n_80, // 0x80

    &n_81, // 0x81

    &n_82, // 0x82

    &n_83, // 0x83

    &nop, // 0x84

    &n_85, // 0x85

    &nop, // 0x86

    &n_87, // 0x87

    &nop, // 0x88

    &nop, // 0x89

    &n_8a, // 0x8A

    &nop, // 0x8B

    &nop, // 0x8C

    &n_8d, // 0x8D

    &nop, // 0x8E

    &n_8f, // 0x8F

    &n_90, // 0x90

    &n_91, // 0x91

    &n_92, // 0x92

    &n_93, // 0x93

    &nop, // 0x94

    &n_95, // 0x95

    &nop, // 0x96

    &n_97, // 0x97

    &n_98, // 0x98

    &n_99, // 0x99

    &n_9a, // 0x9A

    &n_9b, // 0x9B

    &nop, // 0x9C

    &n_9d, // 0x9D

    &nop, // 0x9E

    &n_9f, // 0x9F

    &n_a0, // 0xA0

    &n_a1, // 0xA1

    &n_a2, // 0xA2

    &n_a3, // 0xA3

    &n_a4, // 0xA4

    &n_a5, // 0xA5

    &n_a6, // 0xA6

    &n_a7, // 0xA7

    &n_a8, // 0xA8

    &n_a9, // 0xA9

    &n_aa, // 0xAA

    &nop, // 0xAB

    &n_ac, // 0xAC

    &n_ad, // 0xAD

    &n_ae, // 0xAE

    &n_af, // 0xAF

    &n_b0, // 0xB0

    &n_b1, // 0xB1

    &n_b2, // 0xB2

    &n_b3, // 0xB3

    &n_b4, // 0xB4

    &n_b5, // 0xB5

    &n_b6, // 0xB6

    &n_b7, // 0xB7

    &n_b8, // 0xB8

    &n_b9, // 0xB9

    &n_ba, // 0xBA

    &n_bb, // 0xBB

    &n_bc, // 0xBC

    &n_bd, // 0xBD

    &n_be, // 0xBE

    &n_bf, // 0xBF

    &nop, // 0xC0

    &n_c1, // 0xC1

    &nop, // 0xC2

    &n_c3, // 0xC3

    &nop, // 0xC4

    &n_c5, // 0xC5

    &nop, // 0xC6

    &n_c7, // 0xC7

    &nop, // 0xC8

    &n_c9, // 0xC9

    &nop, // 0xCA

    &nop, // 0xCB

    &nop, // 0xCC

    &n_cd, // 0xCD

    &nop, // 0xCE

    &n_cf, // 0xCF

    &n_d0, // 0xD0

    &n_d1, // 0xD1

    &n_d2, // 0xD2

    &n_d3, // 0xD3

    &nop, // 0xD4

    &n_d5, // 0xD5

    &nop, // 0xD6

    &n_d7, // 0xD7

    &n_d8, // 0xD8

    &n_d9, // 0xD9

    &nop, // 0xDA

    &nop, // 0xDB

    &nop, // 0xDC

    &n_dd, // 0xDD

    &nop, // 0xDE

    &n_df, // 0xDF

    &nop, // 0xE0

    &n_e1, // 0xE1

    &nop, // 0xE2

    &n_e3, // 0xE3

    &nop, // 0xE4

    &n_e5, // 0xE5

    &nop, // 0xE6

    &n_e7, // 0xE7

    &nop, // 0xE8

    &n_e9, // 0xE9

    &n_ea, // 0xEA

    &n_eb, // 0xEB

    &nop, // 0xEC

    &n_ed, // 0xED

    &nop, // 0xEE

    &n_ef, // 0xEF

    &n_f0, // 0xF0

    &n_f1, // 0xF1

    &n_f2, // 0xF2

    &n_f3, // 0xF3

    &nop, // 0xF4

    &n_f5, // 0xF5

    &nop, // 0xF6

    &n_f7, // 0xF7

    &nop, // 0xF8

    &n_f9, // 0xF9

    &nop, // 0xFA

    &nop, // 0xFB

    &nop, // 0xFC

    &n_fd, // 0xFD

    &nop, // 0xFE

    &n_ff // 0xFF

};