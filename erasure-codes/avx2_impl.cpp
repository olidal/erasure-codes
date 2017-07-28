
#include <cstdint>
#include <immintrin.h>

static constexpr size_t n = 20;
static constexpr size_t k = 16;
static constexpr size_t node_bytes = 1024 * 1024 * 64 / 16;

namespace avx2_impl
{
	// Contains lo then hi
	__declspec(align(32)) constexpr uint8_t lohi_table[][2][16] = {
		{ { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } },
		{ { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },{ 0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240 } },
		{ { 0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30 },{ 0,32,64,96,128,160,192,224,29,61,93,125,157,189,221,253 } },
		{ { 0,3,6,5,12,15,10,9,24,27,30,128,20,23,18,17 },{ 0,48,96,80,192,240,160,144,157,173,253,232,93,109,61,13 } },
		{ { 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60 },{ 0,64,128,192,29,93,157,221,58,122,186,250,39,103,167,231 } },
		{ { 0,5,10,15,20,17,30,27,40,152,34,157,60,57,54,51 },{ 0,80,160,240,93,13,253,173,186,117,26,37,231,183,71,23 } },
		{ { 0,6,12,10,24,30,20,18,48,54,60,29,40,46,36,34 },{ 0,96,192,160,157,253,93,61,39,71,231,205,186,218,122,26 } },
		{ { 0,7,14,9,28,27,18,132,56,145,54,150,36,159,21,152 },{ 0,112,224,144,221,173,61,168,167,229,71,149,122,5,77,117 } },
		{ { 0,8,16,24,32,40,48,56,64,72,80,88,96,104,112,120 },{ 0,128,29,157,58,186,39,167,116,244,105,233,78,206,83,211 } },
		{ { 0,9,18,27,36,152,54,145,72,174,45,167,108,188,63,181 },{ 0,144,61,173,122,117,71,229,244,50,234,162,142,15,215,159 } },
		{ { 0,10,20,30,40,34,60,54,80,45,68,39,120,114,108,102 },{ 0,160,93,253,186,26,231,71,105,234,52,74,211,115,142,46 } },
		{ { 0,11,22,128,44,157,29,150,88,167,39,172,58,177,49,186 },{ 0,176,125,232,250,37,205,149,233,162,74,18,135,223,55,111 } },
		{ { 0,12,24,20,48,60,40,36,96,108,120,58,80,92,72,68 },{ 0,192,157,93,39,231,186,122,78,142,211,135,105,169,244,52 } },
		{ { 0,13,26,23,52,57,46,159,104,188,114,177,92,81,35,75 },{ 0,208,189,109,103,183,218,5,206,15,115,223,169,121,10,196 } },
		{ { 0,14,28,18,56,54,36,21,112,63,108,49,72,35,42,45 },{ 0,224,221,61,167,71,122,77,83,215,142,55,244,10,154,234 } },
		{ { 0,15,30,17,60,51,34,152,120,181,102,186,68,75,45,85 },{ 0,240,253,13,231,23,26,117,211,159,46,111,52,196,234,57 } },
		{ { 0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240 },{ 0,29,58,39,116,105,78,83,232,245,210,207,156,129,166,187 } },
		{ { 0,17,34,51,68,85,102,181,136,194,170,211,204,221,119,255 },{ 0,13,26,23,52,57,46,159,104,188,114,177,92,81,35,75 } },
		{ { 0,18,36,54,72,45,108,63,144,65,90,83,216,101,126,119 },{ 0,61,122,71,244,234,142,215,245,100,201,89,0,30,179,35 } },
		{ { 0,19,38,53,76,95,106,121,152,139,190,173,212,199,242,225 },{ 0,45,90,119,180,153,238,195,117,88,47,1,193,236,155,182 } },
		{ { 0,20,40,60,80,68,120,108,160,90,136,78,240,228,216,204 },{ 0,93,186,231,105,52,211,142,210,201,104,148,187,230,0,92 } },
		{ { 0,21,42,63,84,65,126,187,168,208,130,197,252,233,107,195 },{ 0,77,154,215,41,100,179,127,82,129,200,204,123,54,254,172 } },
		{ { 0,22,44,29,88,39,58,49,176,83,78,69,116,127,98,105 },{ 0,125,250,205,233,74,135,55,207,89,148,36,19,163,110,222 } },
		{ { 0,23,46,57,92,75,114,188,184,217,150,206,228,243,101,221 },{ 0,109,218,183,169,196,115,15,79,17,149,124,230,139,30,81 } },
		{ { 0,24,48,40,96,120,80,72,192,216,240,116,160,184,144,136 },{ 0,157,39,186,78,211,105,244,156,0,187,19,210,79,245,104 } },
		{ { 0,25,50,43,100,125,86,169,200,230,250,255,172,212,79,205 },{ 0,141,7,138,14,131,9,66,28,198,27,75,18,193,132,76 } },
		{ { 0,26,52,46,104,114,92,35,208,101,228,127,184,162,70,150 },{ 0,189,103,218,206,115,169,10,129,30,230,163,79,242,20,149 } },
		{ { 0,27,54,152,108,181,45,174,216,239,119,244,90,217,65,194 },{ 0,173,71,117,142,159,234,50,0,86,35,251,201,17,100,188 } },
		{ { 0,28,56,36,112,108,72,42,224,126,216,98,144,70,84,90 },{ 0,221,167,122,83,142,244,154,166,179,0,110,245,20,41,201 } },
		{ { 0,29,58,39,116,105,78,83,232,245,210,207,156,129,166,187 },{ 0,205,135,74,19,222,148,89,38,235,161,108,53,248,178,127 } },
		{ { 0,30,60,34,120,102,68,45,240,119,204,105,136,150,90,170 },{ 0,253,231,26,211,46,52,234,187,35,92,222,104,149,201,114 } },
		{ { 0,31,62,33,124,99,66,160,248,253,198,226,132,155,93,165 },{ 0,237,199,42,147,126,84,210,59,107,252,134,168,69,185,130 } },
		{ { 0,32,64,96,128,160,192,224,29,61,93,125,157,189,221,253 },{ 0,58,116,78,232,210,156,166,205,247,185,131,37,31,81,107 } },
		{ { 0,33,66,99,132,165,198,253,21,26,87,59,145,176,231,242 },{ 0,42,84,126,168,130,252,107,77,189,25,151,229,207,214,155 } },
		{ { 0,34,68,102,136,170,204,119,13,153,73,187,133,167,238,227 },{ 0,26,52,46,104,114,92,35,208,101,228,127,184,162,70,150 } },
		{ { 0,35,70,101,140,175,202,233,5,19,67,48,137,170,207,236 },{ 0,10,20,30,40,34,60,54,80,45,68,39,120,114,108,102 } },
		{ { 0,36,72,108,144,90,216,126,61,130,180,166,173,202,252,238 },{ 0,122,244,142,245,201,0,179,247,200,143,178,1,60,123,70 } },
		{ { 0,37,74,111,148,177,222,251,53,8,127,45,161,132,235,206 },{ 0,106,212,190,181,223,97,11,119,128,163,234,194,168,22,124 } },
		{ { 0,38,76,106,152,190,212,242,45,11,97,71,181,147,249,223 },{ 0,90,180,238,117,47,193,155,234,176,94,2,159,197,43,113 } },
		{ { 0,39,78,105,156,187,210,245,37,1,107,38,185,158,247,208 },{ 0,74,148,222,53,127,161,235,106,16,254,90,95,21,203,129 } },
		{ { 0,40,80,120,160,136,240,216,93,180,13,156,253,213,173,133 },{ 0,186,105,211,210,104,187,0,185,143,208,53,107,209,1,184 } },
		{ { 0,41,82,123,164,141,246,225,85,62,7,23,241,216,223,138 },{ 0,170,73,227,146,56,219,182,57,199,112,109,171,0,113,72 } },
		{ { 0,42,84,126,168,130,252,107,77,189,25,151,229,207,214,155 },{ 0,154,41,179,82,200,123,254,164,31,141,133,246,108,225,69 } },
		{ { 0,43,86,125,172,205,250,230,69,55,135,28,233,97,209,74 },{ 0,138,9,131,18,76,27,198,36,87,152,221,54,94,145,212 } },
		{ { 0,44,88,58,176,78,116,98,125,166,156,138,232,254,196,210 },{ 0,250,233,135,207,148,19,110,131,178,53,72,38,91,220,161 } },
		{ { 0,45,90,119,180,153,238,195,117,88,47,1,193,236,155,182 },{ 0,234,201,35,143,101,70,172,3,233,202,16,140,102,69,175 } },
		{ { 0,46,92,114,184,150,228,101,109,175,49,129,213,251,202,167 },{ 0,218,169,115,79,149,230,30,158,34,55,248,209,11,60,162 } },
		{ { 0,47,94,113,188,147,226,232,101,37,59,10,217,246,205,168 },{ 0,202,137,67,15,197,134,38,30,106,151,160,17,219,76,82 } },
		{ { 0,48,96,80,192,240,160,144,157,173,253,232,93,109,61,13 },{ 0,39,78,105,156,187,210,245,37,1,107,38,185,158,247,208 } },
		{ { 0,49,98,83,196,245,166,197,149,82,247,99,81,48,151,1 },{ 0,55,110,89,220,235,178,204,165,73,203,126,121,39,133,16 } },
		{ { 0,50,100,86,200,250,172,79,141,209,233,227,69,181,158,135 },{ 0,7,14,9,28,27,18,132,56,145,54,150,36,159,21,152 } },
		{ { 0,51,102,85,204,255,170,194,133,91,227,104,73,122,153,28 },{ 0,23,46,57,92,75,114,188,184,217,150,206,228,243,101,221 } },
		{ { 0,52,104,92,208,228,184,70,189,202,213,254,109,89,140,49 },{ 0,103,206,169,129,230,79,20,31,60,209,91,158,249,40,55 } },
		{ { 0,53,106,95,212,225,190,139,181,64,223,117,97,84,11,62 },{ 0,119,238,153,193,182,47,88,159,116,113,3,94,41,176,199 } },
		{ { 0,54,108,45,216,119,90,65,173,195,238,245,180,175,130,153 },{ 0,71,142,234,0,35,201,100,1,172,70,235,143,34,200,101 } },
		{ { 0,55,110,89,220,235,178,204,165,73,203,126,121,39,133,16 },{ 0,87,174,249,65,22,239,92,130,228,44,179,195,74,184,29 } },
		{ { 0,56,112,72,224,216,144,84,221,252,173,196,61,140,168,180 },{ 0,167,83,244,166,0,245,41,81,123,1,220,247,40,82,143 } },
		{ { 0,57,114,75,228,221,150,217,213,118,167,79,49,4,175,122 },{ 0,183,115,196,230,81,149,17,209,51,162,132,55,64,34,243 } },
		{ { 0,58,116,78,232,210,156,166,205,247,185,131,37,31,81,107 },{ 0,135,19,148,38,161,53,178,76,203,95,216,106,237,121,254 } },
		{ { 0,59,118,77,236,215,154,222,197,127,179,68,41,18,161,100 },{ 0,151,51,164,102,241,85,97,204,163,255,52,170,61,194,14 } },
		{ { 0,60,120,68,240,204,136,90,253,238,133,210,13,49,180,73 },{ 0,231,211,52,187,92,104,201,107,70,184,161,208,55,143,228 } },
		{ { 0,61,122,71,244,234,142,215,245,100,201,89,0,30,179,35 },{ 0,247,243,2,251,6,4,241,235,14,12,249,8,253,255,10 } },
		{ { 0,62,124,66,248,198,132,93,237,231,145,217,21,43,186,87 },{ 0,199,147,84,59,252,168,185,118,214,229,17,77,138,111,25 } },
		{ { 0,63,126,65,252,195,130,208,229,109,155,82,25,19,189,88 },{ 0,215,179,100,123,172,200,129,246,158,69,73,141,45,31,233 } },
		{ { 0,64,128,192,29,93,157,221,58,122,186,250,39,103,167,231 },{ 0,116,232,156,205,185,37,81,135,243,111,27,74,62,162,214 } },
		{ { 0,65,130,195,25,88,155,109,50,183,176,246,43,53,218,116 },{ 0,100,200,172,141,233,69,158,7,191,207,219,138,119,33,19 } },
		{ { 0,66,132,198,21,87,145,231,42,52,174,118,63,125,211,249 },{ 0,84,168,252,77,25,229,214,154,103,50,51,215,131,177,43 } },
		{ { 0,67,134,197,17,82,151,106,34,190,164,253,51,112,212,246 },{ 0,68,136,204,13,73,133,238,26,47,146,107,23,83,193,219 } },
		{ { 0,68,136,204,13,73,133,238,26,47,146,107,23,83,193,219 },{ 0,52,104,92,208,228,184,70,189,202,213,254,109,89,140,49 } },
		{ { 0,69,138,207,9,38,131,99,18,165,76,224,27,47,198,106 },{ 0,36,72,108,144,90,216,126,61,130,180,166,173,202,252,238 } },
		{ { 0,70,140,202,5,67,137,207,10,38,134,96,15,73,131,197 },{ 0,20,40,60,80,68,120,108,160,90,136,78,240,228,216,204 } },
		{ { 0,71,142,234,0,35,201,100,1,172,70,235,143,34,200,101 },{ 0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30 } },
		{ { 0,72,144,216,61,180,173,252,122,25,117,81,71,137,229,193 },{ 0,244,245,0,247,143,1,123,243,141,3,121,2,120,246,140 } },
		{ { 0,73,146,219,57,112,171,113,114,147,224,218,75,1,226,144 },{ 0,228,213,49,183,83,98,67,115,197,166,33,196,16,134,245 } },
		{ { 0,74,148,222,53,127,161,235,106,16,254,90,95,21,203,129 },{ 0,212,181,97,119,163,194,22,238,29,91,201,153,77,44,248 } },
		{ { 0,75,150,221,49,122,167,118,98,154,244,209,83,12,236,142 },{ 0,196,149,81,55,243,162,51,110,85,251,145,89,192,102,4 } },
		{ { 0,76,152,212,45,97,181,249,90,22,194,142,119,59,239,163 },{ 0,180,117,193,234,94,159,43,201,125,188,4,35,151,86,226 } },
		{ { 0,77,154,215,41,100,179,127,82,129,200,204,123,54,254,172 },{ 0,164,85,241,170,14,255,163,73,248,28,92,227,71,91,18 } },
		{ { 0,78,156,210,37,107,185,247,74,2,214,76,111,33,243,189 },{ 0,148,53,161,106,254,95,203,212,32,225,180,190,42,139,31 } },
		{ { 0,79,158,209,33,110,191,120,66,136,220,199,99,44,240,178 },{ 0,132,21,145,42,174,63,211,84,104,65,236,126,250,187,239 } },
		{ { 0,80,160,240,93,13,253,173,186,117,26,37,231,183,71,23 },{ 0,105,210,187,185,208,107,1,111,3,189,106,214,191,2,109 } },
		{ { 0,81,162,243,89,4,251,85,178,255,8,174,235,93,170,12 },{ 0,121,242,139,249,64,11,57,239,75,128,50,22,185,114,192 } },
		{ { 0,82,164,246,85,7,241,223,170,124,14,46,255,173,163,9 },{ 0,73,146,219,57,112,171,113,114,147,224,218,75,1,226,144 } },
		{ { 0,83,166,245,81,1,247,82,162,246,2,165,243,80,164,3 },{ 0,89,178,235,121,16,203,73,242,219,32,130,139,105,146,48 } },
		{ { 0,84,168,252,77,25,229,214,154,103,50,51,215,131,177,43 },{ 0,41,82,123,164,141,246,225,85,62,7,23,241,216,223,138 } },
		{ { 0,85,170,255,73,28,227,91,146,237,56,184,219,142,182,36 },{ 0,57,114,75,228,221,150,217,213,118,167,79,49,4,175,122 } },
		{ { 0,86,172,250,69,135,233,209,138,110,19,56,207,194,191,148 },{ 0,9,18,27,36,152,54,145,72,174,45,167,108,188,63,181 } },
		{ { 0,87,174,249,65,22,239,92,130,228,44,179,195,74,184,29 },{ 0,25,50,43,100,125,86,169,200,230,250,255,172,212,79,205 } },
		{ { 0,88,176,116,125,156,232,196,250,81,37,9,205,225,149,185 },{ 0,233,207,19,131,53,38,220,27,121,106,144,76,182,165,95 } },
		{ { 0,89,178,235,121,16,203,73,242,219,32,130,139,105,146,48 },{ 0,249,239,22,195,29,44,228,155,49,58,200,88,222,213,39 } },
		{ { 0,90,180,238,117,47,193,155,234,176,94,2,159,197,43,113 },{ 0,201,143,70,3,202,140,69,6,207,137,32,5,204,138,67 } },
		{ { 0,91,182,237,113,42,199,78,226,210,84,137,147,200,156,126 },{ 0,217,175,118,67,154,236,148,134,161,41,120,197,28,53,179 } },
		{ { 0,92,184,228,109,49,213,202,218,67,98,31,183,235,137,83 },{ 0,169,79,230,158,55,209,60,33,68,110,237,191,22,120,89 } },
		{ { 0,93,186,231,105,52,211,142,210,201,104,148,187,230,0,92 },{ 0,185,111,214,222,103,177,4,161,12,206,181,127,198,8,169 } },
		{ { 0,94,188,226,101,59,217,205,202,74,118,20,175,241,135,77 },{ 0,137,15,134,30,151,17,76,60,212,51,93,34,171,152,164 } },
		{ { 0,95,190,225,97,62,223,64,194,192,124,159,163,252,128,66 },{ 0,153,47,182,94,199,113,116,188,156,147,5,226,123,232,84 } },
		{ { 0,96,192,160,157,253,93,61,39,71,231,205,186,218,122,26 },{ 0,78,156,210,37,107,185,247,74,2,214,76,111,33,243,189 } },
		{ { 0,97,194,163,153,248,91,29,47,39,237,70,182,215,58,21 },{ 0,94,188,226,101,59,217,205,202,74,118,20,175,241,135,77 } },
		{ { 0,98,196,166,149,247,81,151,55,164,243,198,162,96,51,2 },{ 0,110,220,178,165,203,121,133,87,146,139,252,242,78,23,32 } },
		{ { 0,99,198,165,145,242,87,26,63,46,249,77,174,232,52,11 },{ 0,126,252,130,229,155,25,189,215,218,43,164,50,38,103,176 } },
		{ { 0,100,200,172,141,233,69,158,7,191,207,219,138,119,33,19 },{ 0,14,28,18,56,54,36,21,112,63,108,49,72,35,42,45 } },
		{ { 0,101,202,175,137,236,67,19,15,53,197,80,134,227,38,41 },{ 0,30,60,34,120,102,68,45,240,119,204,105,136,150,90,170 } },
		{ { 0,102,204,170,133,227,73,153,23,182,219,208,146,244,47,56 },{ 0,46,92,114,184,150,228,101,109,175,49,129,213,251,202,167 } },
		{ { 0,103,206,169,129,230,79,20,31,60,209,91,158,249,40,55 },{ 0,62,124,66,248,198,132,93,237,231,145,217,21,43,186,87 } },
		{ { 0,104,208,184,189,213,109,140,103,137,183,225,218,178,5,98 },{ 0,206,129,79,31,209,158,40,62,120,191,182,33,239,80,110 } },
		{ { 0,105,210,187,185,208,107,1,111,3,189,106,214,191,2,109 },{ 0,222,161,127,95,129,254,16,190,48,31,238,225,63,32,158 } },
		{ { 0,106,212,190,181,223,97,11,119,128,163,234,194,168,22,124 },{ 0,238,193,47,159,113,94,176,35,232,226,6,188,82,125,147 } },
		{ { 0,107,214,189,177,218,103,6,127,10,169,97,206,165,12,115 },{ 0,254,225,31,223,33,62,96,163,160,66,94,124,130,192,99 } },
		{ { 0,108,216,90,173,238,180,130,71,155,193,247,117,67,25,47 },{ 0,142,0,201,1,70,143,200,2,69,140,203,3,68,141,202 } },
		{ { 0,109,218,183,169,196,115,15,79,17,149,124,230,139,30,81 },{ 0,158,33,191,66,220,99,240,132,13,165,147,198,88,253,121 } },
		{ { 0,110,220,178,165,203,121,133,87,146,139,252,242,78,23,32 },{ 0,174,65,239,130,44,195,184,25,213,88,123,155,148,109,58 } },
		{ { 0,111,222,177,161,206,127,8,95,24,129,119,254,145,16,79 },{ 0,190,97,223,194,124,163,128,153,157,248,35,91,229,29,132 } },
		{ { 0,112,224,144,221,173,61,168,167,229,71,149,122,5,77,117 },{ 0,83,166,245,81,1,247,82,162,246,2,165,243,80,164,3 } },
		{ { 0,113,226,147,217,168,59,37,175,111,77,30,118,7,74,229 },{ 0,67,134,197,17,82,151,106,34,190,164,253,51,112,212,246 } },
		{ { 0,114,228,150,213,167,49,175,183,236,83,158,98,8,67,244 },{ 0,115,230,149,209,162,55,34,191,102,89,21,110,128,68,251 } },
		{ { 0,115,230,149,209,162,55,34,191,102,89,21,110,128,68,251 },{ 0,99,198,165,145,242,87,26,63,46,249,77,174,232,52,11 } },
		{ { 0,116,232,156,205,185,37,81,135,243,111,27,74,62,162,214 },{ 0,19,38,53,76,95,106,121,152,139,190,173,212,199,242,225 } },
		{ { 0,117,234,159,201,188,35,86,143,250,101,8,70,51,172,217 },{ 0,3,6,5,12,15,10,9,24,27,30,128,20,23,18,17 } },
		{ { 0,118,236,154,197,179,41,161,151,254,123,136,82,36,95,200 },{ 0,51,102,85,204,255,170,194,133,91,227,104,73,122,153,28 } },
		{ { 0,119,238,153,193,182,47,88,159,116,113,3,94,41,176,199 },{ 0,35,70,101,140,175,202,233,5,19,67,48,137,170,207,236 } },
		{ { 0,120,240,136,253,133,13,180,231,193,23,185,26,98,117,146 },{ 0,211,187,104,107,184,208,143,214,140,109,95,189,110,3,213 } },
		{ { 0,121,242,139,249,64,11,57,239,75,128,50,22,185,114,192 },{ 0,195,155,88,43,116,176,183,86,196,232,7,125,95,115,156 } },
		{ { 0,122,244,142,245,201,0,179,247,200,143,178,1,60,123,70 },{ 0,243,251,4,235,12,8,255,203,28,24,239,16,231,227,20 } },
		{ { 0,123,246,141,241,138,7,62,255,66,9,57,14,180,124,131 },{ 0,227,219,56,171,72,112,199,75,84,144,183,224,143,147,216 } },
		{ { 0,124,248,132,237,145,21,186,199,211,63,175,42,86,105,174 },{ 0,147,59,168,118,229,77,111,236,177,215,34,154,9,222,50 } },
		{ { 0,125,250,205,233,74,135,55,207,89,148,36,19,163,110,222 },{ 0,131,27,76,54,212,152,87,108,249,181,122,45,226,174,97 } },
		{ { 0,126,252,130,229,155,25,189,215,218,43,164,50,38,103,176 },{ 0,179,123,200,246,69,141,31,241,33,138,146,7,90,62,207 } },
		{ { 0,127,254,129,225,158,31,48,223,80,33,47,62,65,96,191 },{ 0,163,91,248,182,21,237,39,113,105,42,202,199,100,78,63 } },
		{ { 0,128,29,157,58,186,39,167,116,244,105,233,78,206,83,211 },{ 0,232,205,37,135,111,74,162,19,251,222,54,148,124,89,177 } },
		{ { 0,129,31,158,62,191,33,80,124,240,99,113,66,195,160,220 },{ 0,248,237,21,199,63,42,105,147,187,126,67,84,172,210,65 } },
		{ { 0,130,25,155,50,176,43,218,100,115,125,241,86,106,169,232 },{ 0,200,141,69,7,207,138,33,14,99,131,171,9,238,66,38 } },
		{ { 0,131,27,76,54,212,152,87,108,249,181,122,45,226,174,97 },{ 0,216,173,180,71,193,117,25,142,43,159,243,234,134,50,94 } },
		{ { 0,132,21,145,42,174,63,211,84,104,65,236,126,250,187,239 },{ 0,168,77,229,154,50,215,177,41,206,100,102,179,27,127,86 } },
		{ { 0,133,23,146,46,171,57,94,92,226,75,103,114,247,188,224 },{ 0,184,109,213,218,98,183,137,169,134,196,62,115,203,15,166 } },
		{ { 0,134,17,151,34,164,51,212,68,97,85,231,102,224,181,241 },{ 0,136,13,133,26,146,23,193,52,94,57,214,46,166,159,171 } },
		{ { 0,135,19,148,38,161,53,178,76,203,95,216,106,237,121,254 },{ 0,152,45,181,90,194,119,239,180,44,153,0,238,118,195,91 } },
		{ { 0,136,13,133,26,146,23,193,52,94,57,214,46,166,159,171 },{ 0,104,208,184,189,213,109,140,103,137,183,225,218,178,5,98 } },
		{ { 0,137,15,134,30,151,17,76,60,212,51,93,34,171,152,164 },{ 0,120,240,136,253,133,13,180,231,193,23,185,26,98,117,146 } },
		{ { 0,138,9,131,18,76,27,198,36,87,152,221,54,94,145,212 },{ 0,72,144,216,61,180,173,252,122,25,117,81,71,137,229,193 } },
		{ { 0,139,11,64,22,192,128,75,44,221,157,86,29,214,150,93 },{ 0,88,176,116,125,156,232,196,250,81,37,9,205,225,149,185 } },
		{ { 0,140,5,137,10,134,15,131,20,76,17,192,30,146,27,151 },{ 0,40,80,120,160,136,240,216,93,180,13,156,253,213,173,133 } },
		{ { 0,141,7,138,14,131,9,66,28,198,27,75,18,193,132,76 },{ 0,56,112,72,224,216,144,84,221,252,173,196,61,140,168,180 } },
		{ { 0,142,0,201,1,70,143,200,2,69,140,203,3,68,141,202 },{ 0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60 } },
		{ { 0,143,3,140,6,137,5,138,12,131,15,64,10,133,9,134 },{ 0,24,48,40,96,120,80,72,192,216,240,116,160,184,144,136 } },
		{ { 0,144,61,173,122,117,71,229,244,50,234,162,142,15,215,159 },{ 0,245,247,1,243,3,2,246,251,7,6,242,4,240,241,5 } },
		{ { 0,145,63,174,126,239,65,104,252,184,195,41,130,135,208,44 },{ 0,229,215,50,179,86,100,206,123,79,172,170,200,152,129,250 } },
		{ { 0,146,57,171,114,224,75,226,228,59,221,169,150,2,217,61 },{ 0,213,183,98,115,166,196,134,230,151,81,66,149,32,17,247 } },
		{ { 0,147,59,168,118,229,77,111,236,177,215,34,154,9,222,50 },{ 0,197,151,82,51,246,164,190,102,223,241,26,85,144,97,7 } },
		{ { 0,148,53,161,106,254,95,203,212,32,225,180,190,42,139,31 },{ 0,181,119,194,238,91,153,44,193,58,182,143,47,154,88,237 } },
		{ { 0,149,55,162,110,251,89,102,220,170,235,63,178,157,204,8 },{ 0,165,87,242,174,11,249,46,65,114,22,215,239,37,92,128 } },
		{ { 0,150,49,167,98,244,83,236,196,41,245,191,166,24,197,0 },{ 0,149,55,162,110,251,89,102,220,170,235,63,178,157,204,8 } },
		{ { 0,151,51,164,102,241,85,97,204,163,255,52,170,61,194,14 },{ 0,133,23,146,46,171,57,94,92,226,75,103,114,247,188,224 } },
		{ { 0,152,45,181,90,194,119,239,180,44,153,0,238,118,195,91 },{ 0,117,234,159,201,188,35,86,143,250,101,8,70,51,172,217 } },
		{ { 0,153,47,182,94,199,113,116,188,156,147,5,226,123,232,84 },{ 0,101,202,175,137,236,67,19,15,53,197,80,134,227,38,41 } },
		{ { 0,154,41,179,82,200,123,254,164,31,141,133,246,108,225,69 },{ 0,85,170,255,73,28,227,91,146,237,56,184,219,142,182,36 } },
		{ { 0,155,43,176,86,232,125,115,172,149,205,14,250,190,230,37 },{ 0,69,138,207,9,38,131,99,18,165,76,224,27,47,198,106 } },
		{ { 0,156,37,185,74,214,111,243,148,4,177,152,222,66,251,103 },{ 0,53,106,95,212,225,190,139,181,64,223,117,97,84,11,62 } },
		{ { 0,157,39,186,78,211,105,244,156,0,187,19,210,79,245,104 },{ 0,37,74,111,148,177,222,251,53,8,127,45,161,132,235,206 } },
		{ { 0,158,33,191,66,220,99,240,132,13,165,147,198,88,253,121 },{ 0,21,42,63,84,65,126,187,168,208,130,197,252,233,107,195 } },
		{ { 0,159,35,188,70,217,101,250,140,135,175,24,202,85,233,118 },{ 0,5,10,15,20,17,30,27,40,152,34,157,60,57,54,51 } },
		{ { 0,160,93,253,186,26,231,71,105,234,52,74,211,115,142,46 },{ 0,210,185,107,111,189,214,2,222,6,103,212,177,99,4,218 } },
		{ { 0,161,95,254,190,31,225,32,97,96,62,193,223,126,64,33 },{ 0,194,153,91,47,237,182,58,94,78,199,140,113,179,116,42 } },
		{ { 0,162,89,251,178,8,235,170,121,227,16,65,203,186,73,24 },{ 0,242,249,11,239,128,22,114,195,150,29,100,44,111,228,157 } },
		{ { 0,163,91,248,182,21,237,39,113,105,42,202,199,100,78,63 },{ 0,226,217,59,175,77,118,74,67,222,154,60,236,14,148,215 } },
		{ { 0,164,85,241,170,14,255,163,73,248,28,92,227,71,91,18 },{ 0,146,57,171,114,224,75,226,228,59,221,169,150,2,217,61 } },
		{ { 0,165,87,242,174,11,249,46,65,114,22,215,239,37,92,128 },{ 0,130,25,155,50,176,43,218,100,115,125,241,86,106,169,232 } },
		{ { 0,166,81,247,162,2,243,164,89,241,4,87,251,160,85,6 },{ 0,178,121,203,242,32,139,146,249,171,64,25,11,210,57,96 } },
		{ { 0,167,83,244,166,0,245,41,81,123,1,220,247,40,82,143 },{ 0,162,89,251,178,8,235,170,121,227,16,65,203,186,73,24 } },
		{ { 0,168,77,229,154,50,215,177,41,206,100,102,179,27,127,86 },{ 0,82,164,246,85,7,241,223,170,124,14,46,255,173,163,9 } },
		{ { 0,169,79,230,158,55,209,60,33,68,110,237,191,22,120,89 },{ 0,66,132,198,21,87,145,231,42,52,174,118,63,125,211,249 } },
		{ { 0,170,73,227,146,56,219,182,57,199,112,109,171,0,113,72 },{ 0,114,228,150,213,167,49,175,183,236,83,158,98,8,67,244 } },
		{ { 0,171,75,224,150,61,221,59,49,77,122,230,167,6,118,71 },{ 0,98,196,166,149,247,81,151,55,164,243,198,162,96,51,2 } },
		{ { 0,172,69,233,138,19,207,191,9,220,38,112,131,153,99,53 },{ 0,18,36,54,72,45,108,63,144,65,90,83,216,101,126,119 } },
		{ { 0,173,71,117,142,159,234,50,0,86,35,251,201,17,100,188 },{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 } },
		{ { 0,174,65,239,130,44,195,184,25,213,88,123,155,148,109,58 },{ 0,50,100,86,200,250,172,79,141,209,233,227,69,181,158,135 } },
		{ { 0,175,67,236,134,41,197,53,17,95,82,240,151,56,106,123 },{ 0,34,68,102,136,170,204,119,13,153,73,187,133,167,238,227 } },
		{ { 0,176,125,232,250,37,205,149,233,162,74,18,135,223,55,111 },{ 0,207,131,38,27,106,76,165,54,242,212,61,152,113,87,190 } },
		{ { 0,177,127,206,254,79,129,24,225,40,158,153,31,174,48,209 },{ 0,223,163,124,91,132,248,157,182,186,21,101,237,50,39,145 } },
		{ { 0,178,121,203,242,32,139,146,249,171,64,25,11,210,57,96 },{ 0,239,195,44,155,58,88,213,43,98,116,141,176,161,183,78 } },
		{ { 0,179,123,200,246,69,141,31,241,33,138,146,7,90,62,207 },{ 0,255,227,28,219,36,56,237,171,42,72,213,112,201,199,108 } },
		{ { 0,180,117,193,234,94,159,43,201,125,188,4,35,151,86,226 },{ 0,143,3,140,6,137,5,138,12,131,15,64,10,133,9,134 } },
		{ { 0,181,119,194,238,91,153,44,193,58,182,143,47,154,88,237 },{ 0,159,35,188,70,217,101,250,140,135,175,24,202,85,233,118 } },
		{ { 0,182,113,199,226,84,147,156,217,185,168,15,59,141,37,252 },{ 0,175,67,236,134,41,197,53,17,95,82,240,151,56,106,123 } },
		{ { 0,183,115,196,230,81,149,17,209,51,162,132,55,64,34,243 },{ 0,191,99,220,198,121,165,13,145,23,242,168,87,116,26,139 } },
		{ { 0,184,109,213,218,98,183,137,169,134,196,62,115,203,15,166 },{ 0,79,158,209,33,110,191,120,66,136,220,199,99,44,240,178 } },
		{ { 0,185,111,214,222,103,177,4,161,12,206,181,127,198,8,169 },{ 0,95,190,225,97,62,223,64,194,192,124,159,163,252,128,66 } },
		{ { 0,186,105,211,210,104,187,0,185,143,208,53,107,209,1,184 },{ 0,111,222,177,161,206,127,8,95,24,129,119,254,145,16,79 } },
		{ { 0,187,107,208,214,109,189,3,177,5,218,190,103,220,6,183 },{ 0,127,254,129,225,158,31,48,223,80,33,47,62,65,96,191 } },
		{ { 0,188,101,217,202,118,175,135,137,148,236,40,67,255,19,154 },{ 0,15,30,17,60,51,34,152,120,181,102,186,68,75,45,85 } },
		{ { 0,189,103,218,206,115,169,10,129,30,230,163,79,242,20,149 },{ 0,31,62,33,124,99,66,160,248,253,198,226,132,155,93,165 } },
		{ { 0,190,97,223,194,124,163,128,153,157,248,35,91,229,29,132 },{ 0,47,94,113,188,147,226,232,101,37,59,10,217,246,205,168 } },
		{ { 0,191,99,220,198,121,165,13,145,23,242,168,87,116,26,139 },{ 0,63,126,65,252,195,130,208,229,109,155,82,25,19,189,88 } },
		{ { 0,192,157,93,39,231,186,122,78,142,211,135,105,169,244,52 },{ 0,156,37,185,74,214,111,243,148,4,177,152,222,66,251,103 } },
		{ { 0,193,159,94,35,226,188,125,70,205,217,12,101,164,250,59 },{ 0,140,5,137,10,134,15,131,20,76,17,192,30,146,27,151 } },
		{ { 0,194,153,91,47,237,182,58,94,78,199,140,113,179,116,42 },{ 0,188,101,217,202,118,175,135,137,148,236,40,67,255,19,154 } },
		{ { 0,195,155,88,43,116,176,183,86,196,232,7,125,95,115,156 },{ 0,172,69,233,138,19,207,191,9,220,38,112,131,153,99,53 } },
		{ { 0,196,149,81,55,243,162,51,110,85,251,145,89,192,102,4 },{ 0,220,165,121,87,139,242,23,174,57,11,229,249,156,46,64 } },
		{ { 0,197,151,82,51,246,164,190,102,223,241,26,85,144,97,7 },{ 0,204,133,73,23,219,146,47,46,113,171,189,57,245,94,112 } },
		{ { 0,198,145,87,63,249,174,52,126,92,239,154,65,205,104,22 },{ 0,252,229,25,215,43,50,103,179,169,86,85,100,76,206,125 } },
		{ { 0,199,147,84,59,252,168,185,118,214,229,17,77,138,111,25 },{ 0,236,197,41,151,123,82,95,51,225,246,13,164,72,190,141 } },
		{ { 0,200,141,69,7,207,138,33,14,99,131,171,9,238,66,38 },{ 0,28,56,36,112,108,72,42,224,126,216,98,144,70,84,90 } },
		{ { 0,201,143,70,3,202,140,69,6,207,137,32,5,204,138,67 },{ 0,12,24,20,48,60,40,36,96,108,120,58,80,92,72,68 } },
		{ { 0,202,137,67,15,197,134,38,30,106,151,160,17,219,76,82 },{ 0,60,120,68,240,204,136,90,253,238,133,210,13,49,180,73 } },
		{ { 0,203,139,32,11,96,64,171,22,224,192,43,128,107,75,160 },{ 0,44,88,58,176,78,116,98,125,166,156,138,232,254,196,210 } },
		{ { 0,204,133,73,23,219,146,47,46,113,171,189,57,245,94,112 },{ 0,92,184,228,109,49,213,202,218,67,98,31,183,235,137,83 } },
		{ { 0,205,135,74,19,222,148,89,38,235,161,108,53,248,178,127 },{ 0,76,152,212,45,97,181,249,90,22,194,142,119,59,239,163 } },
		{ { 0,206,129,79,31,209,158,40,62,120,191,182,33,239,80,110 },{ 0,124,248,132,237,145,21,186,199,211,63,175,42,86,105,174 } },
		{ { 0,207,131,38,27,106,76,165,54,242,212,61,152,113,87,190 },{ 0,108,216,90,173,238,180,130,71,155,193,247,117,67,25,47 } },
		{ { 0,208,189,109,103,183,218,5,206,15,115,223,169,121,10,196 },{ 0,129,31,158,62,191,33,80,124,240,99,113,66,195,160,220 } },
		{ { 0,209,191,110,99,178,220,136,198,133,121,84,165,58,13,203 },{ 0,145,63,174,126,239,65,104,252,184,195,41,130,135,208,44 } },
		{ { 0,210,185,107,111,189,214,2,222,6,103,212,177,99,4,218 },{ 0,161,95,254,190,31,225,32,97,96,62,193,223,126,64,33 } },
		{ { 0,211,187,104,107,184,208,143,214,140,109,95,189,110,3,213 },{ 0,177,127,206,254,79,129,24,225,40,158,153,31,174,48,209 } },
		{ { 0,212,181,97,119,163,194,22,238,29,91,201,153,77,44,248 },{ 0,193,159,94,35,226,188,125,70,205,217,12,101,164,250,59 } },
		{ { 0,213,183,98,115,166,196,134,230,151,81,66,149,32,17,247 },{ 0,209,191,110,99,178,220,136,198,133,121,84,165,58,13,203 } },
		{ { 0,214,177,103,127,169,206,12,254,20,79,194,129,87,24,230 },{ 0,225,223,62,163,66,124,192,91,93,132,188,248,25,157,198 } },
		{ { 0,215,179,100,123,172,200,129,246,158,69,73,141,45,31,233 },{ 0,241,255,14,227,18,28,248,219,21,36,228,56,234,237,54 } },
		{ { 0,216,173,180,71,193,117,25,142,43,159,243,234,134,50,94 },{ 0,0,1,143,2,140,3,141,4,138,5,139,6,136,7,137 } },
		{ { 0,217,175,118,67,154,236,148,134,161,41,120,197,28,53,179 },{ 0,17,34,51,68,85,102,181,136,194,170,211,204,221,119,255 } },
		{ { 0,218,169,115,79,149,230,30,158,34,55,248,209,11,60,162 },{ 0,33,66,99,132,165,198,253,21,26,87,59,145,176,231,242 } },
		{ { 0,219,171,112,75,144,224,147,150,168,61,115,221,3,59,173 },{ 0,49,98,83,196,245,166,197,149,82,247,99,81,48,151,1 } },
		{ { 0,220,165,121,87,139,242,23,174,57,11,229,249,156,46,64 },{ 0,65,130,195,25,88,155,109,50,183,176,246,43,53,218,116 } },
		{ { 0,221,167,122,83,142,244,154,166,179,0,110,245,20,41,201 },{ 0,81,162,243,89,4,251,85,178,255,8,174,235,93,170,12 } },
		{ { 0,222,161,127,95,129,254,16,190,48,31,238,225,63,32,158 },{ 0,97,194,163,153,248,91,29,47,39,237,70,182,215,58,21 } },
		{ { 0,223,163,124,91,132,248,157,182,186,21,101,237,50,39,145 },{ 0,113,226,147,217,168,59,37,175,111,77,30,118,7,74,229 } },
		{ { 0,224,221,61,167,71,122,77,83,215,142,55,244,10,154,234 },{ 0,166,81,247,162,2,243,164,89,241,4,87,251,160,85,6 } },
		{ { 0,225,223,62,163,66,124,192,91,93,132,188,248,25,157,198 },{ 0,182,113,199,226,84,147,156,217,185,168,15,59,141,37,252 } },
		{ { 0,226,217,59,175,77,118,74,67,222,154,60,236,14,148,215 },{ 0,134,17,151,34,164,51,212,68,97,85,231,102,224,181,241 } },
		{ { 0,227,219,56,171,72,112,199,75,84,144,183,224,143,147,216 },{ 0,150,49,167,98,244,83,236,196,41,245,191,166,24,197,0 } },
		{ { 0,228,213,49,183,83,98,67,115,197,166,33,196,16,134,245 },{ 0,230,209,55,191,89,110,68,99,204,178,42,220,29,136,235 } },
		{ { 0,229,215,50,179,86,100,206,123,79,172,170,200,152,129,250 },{ 0,246,241,7,255,9,14,124,227,132,18,114,28,117,248,27 } },
		{ { 0,230,209,55,191,89,110,68,99,204,178,42,220,29,136,235 },{ 0,198,145,87,63,249,174,52,126,92,239,154,65,205,104,22 } },
		{ { 0,231,211,52,187,92,104,201,107,70,184,161,208,55,143,228 },{ 0,214,177,103,127,169,206,12,254,20,79,194,129,87,24,230 } },
		{ { 0,232,205,37,135,111,74,162,19,251,222,54,148,124,89,177 },{ 0,38,76,106,152,190,212,242,45,11,97,71,181,147,249,223 } },
		{ { 0,233,207,19,131,53,38,220,27,121,106,144,76,182,165,95 },{ 0,54,108,45,216,119,90,65,173,195,238,245,180,175,130,153 } },
		{ { 0,234,201,35,143,101,70,172,3,233,202,16,140,102,69,175 },{ 0,6,12,10,24,30,20,18,48,54,60,29,40,46,36,34 } },
		{ { 0,235,203,16,139,48,32,219,11,112,96,155,64,187,171,80 },{ 0,22,44,29,88,39,58,49,176,83,78,69,116,127,98,105 } },
		{ { 0,236,197,41,151,123,82,95,51,225,246,13,164,72,190,141 },{ 0,102,204,170,133,227,73,153,23,182,219,208,146,244,47,56 } },
		{ { 0,237,199,42,147,126,84,210,59,107,252,134,168,69,185,130 },{ 0,118,236,154,197,179,41,161,151,254,123,136,82,36,95,200 } },
		{ { 0,238,193,47,159,113,94,176,35,232,226,6,188,82,125,147 },{ 0,70,140,202,5,67,137,207,10,38,134,96,15,73,131,197 } },
		{ { 0,239,195,44,155,58,88,213,43,98,116,141,176,161,183,78 },{ 0,86,172,250,69,135,233,209,138,110,19,56,207,194,191,148 } },
		{ { 0,240,253,13,231,23,26,117,211,159,46,111,52,196,234,57 },{ 0,187,107,208,214,109,189,3,177,5,218,190,103,220,6,183 } },
		{ { 0,241,255,14,227,18,28,248,219,21,36,228,56,234,237,54 },{ 0,171,75,224,150,61,221,59,49,77,122,230,167,6,118,71 } },
		{ { 0,242,249,11,239,128,22,114,195,150,29,100,44,111,228,157 },{ 0,155,43,176,86,232,125,115,172,149,205,14,250,190,230,37 } },
		{ { 0,243,251,4,235,12,8,255,203,28,24,239,16,231,227,20 },{ 0,139,11,64,22,192,128,75,44,221,157,86,29,214,150,93 } },
		{ { 0,244,245,0,247,143,1,123,243,141,3,121,2,120,246,140 },{ 0,251,235,8,203,24,16,227,139,56,48,195,32,211,219,40 } },
		{ { 0,245,247,1,243,3,2,246,251,7,6,242,4,240,241,5 },{ 0,235,203,16,139,48,32,219,11,112,96,155,64,187,171,80 } },
		{ { 0,246,241,7,255,9,14,124,227,132,18,114,28,117,248,27 },{ 0,219,171,112,75,144,224,147,150,168,61,115,221,3,59,173 } },
		{ { 0,247,243,2,251,6,4,241,235,14,12,249,8,253,255,10 },{ 0,203,139,32,11,96,64,171,22,224,192,43,128,107,75,160 } },
		{ { 0,248,237,21,199,63,42,105,147,187,126,67,84,172,210,65 },{ 0,59,118,77,236,215,154,222,197,127,179,68,41,18,161,100 } },
		{ { 0,249,239,22,195,29,44,228,155,49,58,200,88,222,213,39 },{ 0,43,86,125,172,205,250,230,69,55,135,28,233,97,209,74 } },
		{ { 0,250,233,135,207,148,19,110,131,178,53,72,38,91,220,161 },{ 0,27,54,152,108,181,45,174,216,239,119,244,90,217,65,194 } },
		{ { 0,251,235,8,203,24,16,227,139,56,48,195,32,211,219,40 },{ 0,11,22,128,44,157,29,150,88,167,39,172,58,177,49,186 } },
		{ { 0,252,229,25,215,43,50,103,179,169,86,85,100,76,206,125 },{ 0,123,246,141,241,138,7,62,255,66,9,57,14,180,124,131 } },
		{ { 0,253,231,26,211,46,52,234,187,35,92,222,104,149,201,114 },{ 0,107,214,189,177,218,103,6,127,10,169,97,206,165,12,115 } },
		{ { 0,254,225,31,223,33,62,96,163,160,66,94,124,130,192,99 },{ 0,91,182,237,113,42,199,78,226,210,84,137,147,200,156,126 } },
		{ { 0,255,227,28,219,36,56,237,171,42,72,213,112,201,199,108 },{ 0,75,150,221,49,122,167,118,98,154,244,209,83,12,236,142 } }
	};

	void aligned_zero(uint8_t* dst, size_t size)
	{
		__m256i zero = _mm256_set1_epi8(0);

		for (size_t i = 0; i < size; i += sizeof(__m256i))
		{
			_mm256_store_si256((__m256i*)(dst + i), zero);
		}
	}

	// data and parity must be 32 byte aligned
	void encode(const uint8_t* data, uint8_t* parity)
	{
		aligned_zero(parity, node_bytes * (n - k));

		__m256i lomask = _mm256_set1_epi8(0x0F);
		__m256i himask = _mm256_set1_epi8((char)0xF0);

		for (size_t c = 0; c < k; ++c)
		{
			const __m256i* in = (const __m256i*)(data + node_bytes + c);

			__m256i lot = _mm256_load_si256((const __m256i*)lohi_table[c][0]);
			__m256i hit = _mm256_srli_si256(lot, 16);

			for (size_t row = 0; row < n - k; ++row)
			{
				__m256i* out = (__m256i*)(parity + node_bytes * row);

				for (size_t i = 0; i < node_bytes / sizeof(__m256i); ++i)
				{
					// Initial packed value that will be multiplied by c
					__m256i A = _mm256_load_si256(&in[i]);

					// 4 low bits of each byte of A
					__m256i lo = _mm256_and_si256(A, lomask);
					lo = _mm256_shuffle_epi8(lo, lot);

					// 4 high bits of each byte of A
					__m256i hi = _mm256_and_si256(A, himask);
					hi = _mm256_srli_epi64(hi, 4);
					hi = _mm256_shuffle_epi8(hi, hit);

					// Final value c * A for all bytes in A
					__m256i result = _mm256_xor_si256(hi, lo);

					_mm256_store_si256(&out[i], result);
				}
			}
		}
	}
}