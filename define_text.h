constexpr auto YS_UTF32 = 12000;

constexpr auto YS_UTF8 = 65001;

/// <summary>the address of first character in Adol Daily </summary>
constexpr auto DC_R_FIRST = 0x004DBBE4;
/// <summary>the address of last character in Adol Daily </summary>
constexpr auto DC_R_LAST = 0x004DBF8C;
/// <summary>the count of character in Adol Daily </summary>
constexpr auto DC_R_COUNT = 86;
/// <summary>the address of first character describtion in Adol Daily </summary>
constexpr auto DC_R_DES_FIRST = 0x004DBFA0;
/// <summary>the address of last character describtion in Adol Daily </summary>
constexpr auto DC_R_DES_LAST = 0x004E0052;

/// <summary>the address of first monster in Adol Daily </summary>
constexpr auto DC_M_FIRST = 0x004E00C4;
/// <summary>the address of last monster in Adol Daily </summary>
constexpr auto DC_M_LAST = 0x004E0254;
/// <summary>the count of monster in Adol Daily </summary>
constexpr auto DC_M_COUNT = 43;
/// <summary>the address of first monster describtion in Adol Daily </summary>
constexpr auto DC_M_DES_FIRST = 0x004E0260;
/// <summary>the address of last monster describtion in Adol Daily </summary>
constexpr auto DC_M_DES_LAST = 0x004E1DE8;

/// <summary>the end of one string </summary>
constexpr auto CHAR_END = '\0';

/// <summary>Address of the dictionary of all character and monster address, per address use double words </summary>
constexpr auto DC_A_DIC_ADDRESS = 0x004D7400;

/// <summary>Address of the dictionary of character's address, per address use double words </summary>
constexpr auto DC_M_DIC_ADDRESS = 0x004D7270;

/// <summary>Address of the dictionary of monster's describtion address, per address use double words </summary>
constexpr auto DC_M_DES_DIC_ADDRESS = 0x004D7520;

constexpr auto YS1_EXE_CSV_PATH = "ys1_exe_text.csv";

constexpr auto YS1_TEXT_INI = "text.ini";  //ah, twnkey you are my god!

constexpr auto YS1_INI_SPLIT = " ";