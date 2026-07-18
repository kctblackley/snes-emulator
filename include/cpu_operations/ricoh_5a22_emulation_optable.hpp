#pragma once
#include "ricoh_5a22_operations.hpp"

constexpr Optable emulation_optable = {

    &e_00, // 0x00

    &e_01, // 0x01

    &e_02, // 0x02

    &e_03, // 0x03

    &e_04, // 0x04

    &e_05, // 0x05

    &e_06, // 0x06

    &e_07, // 0x07

    &e_08, // 0x08

    &e_09, // 0x09

    &e_0a, // 0x0A

    &e_0b, // 0x0B

    &e_0c, // 0x0C

    &e_0d, // 0x0D

    &e_0e, // 0x0E

    &e_0f, // 0x0F

    &e_10, // 0x10

    &e_11, // 0x11

    &e_12, // 0x12

    &e_13, // 0x13

    &e_14, // 0x14

    &e_15, // 0x15

    &e_16, // 0x16

    &e_17, // 0x17

    &e_18, // 0x18

    &e_19, // 0x19

    &e_1a, // 0x1A

    &e_1b, // 0x1B

    &e_1c, // 0x1C

    &e_1d, // 0x1D

    &e_1e, // 0x1E

    &e_1f, // 0x1F

    &e_20, // 0x20

    &e_21, // 0x21

    &e_22, // 0x22

    &e_23, // 0x23

    &e_24, // 0x24

    &e_25, // 0x25

    &e_26, // 0x26

    &e_27, // 0x27

    &e_28, // 0x28

    &e_29, // 0x29

    &e_2a, // 0x2A

    &e_2b, // 0x2B

    &e_2c, // 0x2C

    &e_2d, // 0x2D

    &e_2e, // 0x2E

    &e_2f, // 0x2F

    &e_30, // 0x30

    &e_31, // 0x31

    &e_32, // 0x32

    &e_33, // 0x33

    &e_34, // 0x34

    &e_35, // 0x35

    &e_36, // 0x36

    &e_37, // 0x37

    &e_38, // 0x38

    &e_39, // 0x39

    &e_3a, // 0x3A

    &e_3b, // 0x3B

    &e_3c, // 0x3C

    &e_3d, // 0x3D

    &e_3e, // 0x3E

    &e_3f, // 0x3F

    &e_40, // 0x40

    &e_41, // 0x41

    &e_42, // 0x42

    &e_43, // 0x43

    &e_44, // 0x44

    &e_45, // 0x45

    &e_46, // 0x46

    &e_47, // 0x47

    &e_48, // 0x48

    &e_49, // 0x49

    &e_4a, // 0x4A

    &e_4b, // 0x4B

    &e_4c, // 0x4C

    &e_4d, // 0x4D

    &e_4e, // 0x4E

    &e_4f, // 0x4F

    &e_50, // 0x50

    &e_51, // 0x51

    &e_52, // 0x52

    &e_53, // 0x53

    &e_54, // 0x54

    &e_55, // 0x55

    &e_56, // 0x56

    &e_57, // 0x57

    &e_58, // 0x58

    &e_59, // 0x59

    &e_5a, // 0x5A

    &e_5b, // 0x5B

    &e_5c, // 0x5C

    &e_5d, // 0x5D

    &e_5e, // 0x5E

    &e_5f, // 0x5F

    &e_60, // 0x60

    &e_61, // 0x61

    &e_62, // 0x62

    &e_63, // 0x63

    &e_64, // 0x64

    &e_65, // 0x65

    &e_66, // 0x66

    &e_67, // 0x67

    &e_68, // 0x68

    &e_69, // 0x69

    &e_6a, // 0x6A

    &e_6b, // 0x6B

    &e_6c, // 0x6C

    &e_6d, // 0x6D

    &e_6e, // 0x6E

    &e_6f, // 0x6F

    &e_70, // 0x70

    &e_71, // 0x71

    &e_72, // 0x72

    &e_73, // 0x73

    &e_74, // 0x74

    &e_75, // 0x75

    &e_76, // 0x76

    &e_77, // 0x77

    &e_78, // 0x78

    &e_79, // 0x79

    &e_7a, // 0x7A

    &e_7b, // 0x7B

    &e_7c, // 0x7C

    &e_7d, // 0x7D

    &e_7e, // 0x7E

    &e_7f, // 0x7F

    &e_80, // 0x80

    &e_81, // 0x81

    &e_82, // 0x82

    &e_83, // 0x83

    &e_84, // 0x84

    &e_85, // 0x85

    &e_86, // 0x86

    &e_87, // 0x87

    &e_88, // 0x88

    &e_89, // 0x89

    &e_8a, // 0x8A

    &e_8b, // 0x8B

    &e_8c, // 0x8C

    &e_8d, // 0x8D

    &e_8e, // 0x8E

    &e_8f, // 0x8F

    &e_90, // 0x90

    &e_91, // 0x91

    &e_92, // 0x92

    &e_93, // 0x93

    &e_94, // 0x94

    &e_95, // 0x95

    &e_96, // 0x96

    &e_97, // 0x97

    &e_98, // 0x98

    &e_99, // 0x99

    &e_9a, // 0x9A

    &e_9b, // 0x9B

    &e_9c, // 0x9C

    &e_9d, // 0x9D

    &e_9e, // 0x9E

    &e_9f, // 0x9F

    &e_a0, // 0xA0

    &e_a1, // 0xA1

    &e_a2, // 0xA2

    &e_a3, // 0xA3

    &e_a4, // 0xA4

    &e_a5, // 0xA5

    &e_a6, // 0xA6

    &e_a7, // 0xA7

    &e_a8, // 0xA8

    &e_a9, // 0xA9

    &e_aa, // 0xAA

    &e_ab, // 0xAB

    &e_ac, // 0xAC

    &e_ad, // 0xAD

    &e_ae, // 0xAE

    &e_af, // 0xAF

    &e_b0, // 0xB0

    &e_b1, // 0xB1

    &e_b2, // 0xB2

    &e_b3, // 0xB3

    &e_b4, // 0xB4

    &e_b5, // 0xB5

    &e_b6, // 0xB6

    &e_b7, // 0xB7

    &e_b8, // 0xB8

    &e_b9, // 0xB9

    &e_ba, // 0xBA

    &e_bb, // 0xBB

    &e_bc, // 0xBC

    &e_bd, // 0xBD

    &e_be, // 0xBE

    &e_bf, // 0xBF

    &e_c0, // 0xC0

    &e_c1, // 0xC1

    &e_c2, // 0xC2

    &e_c3, // 0xC3

    &e_c4, // 0xC4

    &e_c5, // 0xC5

    &e_c6, // 0xC6

    &e_c7, // 0xC7

    &e_c8, // 0xC8

    &e_c9, // 0xC9

    &e_ca, // 0xCA

    &e_cb, // 0xCB

    &e_cc, // 0xCC

    &e_cd, // 0xCD

    &e_ce, // 0xCE

    &e_cf, // 0xCF

    &e_d0, // 0xD0

    &e_d1, // 0xD1

    &e_d2, // 0xD2

    &e_d3, // 0xD3

    &e_d4, // 0xD4

    &e_d5, // 0xD5

    &e_d6, // 0xD6

    &e_d7, // 0xD7

    &e_d8, // 0xD8

    &e_d9, // 0xD9

    &e_da, // 0xDA

    &e_db, // 0xDB

    &e_dc, // 0xDC

    &e_dd, // 0xDD

    &e_de, // 0xDE

    &e_df, // 0xDF

    &e_e0, // 0xE0

    &e_e1, // 0xE1

    &e_e2, // 0xE2

    &e_e3, // 0xE3

    &e_e4, // 0xE4

    &e_e5, // 0xE5

    &e_e6, // 0xE6

    &e_e7, // 0xE7

    &e_e8, // 0xE8

    &e_e9, // 0xE9

    &e_ea, // 0xEA

    &e_eb, // 0xEB

    &e_ec, // 0xEC

    &e_ed, // 0xED

    &e_ee, // 0xEE

    &e_ef, // 0xEF

    &e_f0, // 0xF0

    &e_f1, // 0xF1

    &e_f2, // 0xF2

    &e_f3, // 0xF3

    &e_f4, // 0xF4

    &e_f5, // 0xF5

    &e_f6, // 0xF6

    &e_f7, // 0xF7

    &e_f8, // 0xF8

    &e_f9, // 0xF9

    &e_fa, // 0xFA

    &e_fb, // 0xFB

    &e_fc, // 0xFC

    &e_fd, // 0xFD

    &e_fe, // 0xFE

    &e_ff, // 0xFF

    &e_nmi, // NMI interrupt routine

    &e_irq // IRQ interrupt routine
};