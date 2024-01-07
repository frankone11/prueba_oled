#define ADDR 		0x78U
#define MAX_DAT		32U
#define BUFFER_TAM	1024U

bool Oled_DameBuffer(uint8_t *buf, uint16_t tam);
bool Oled_ColocaBuffer(const uint8_t *buf, uint16_t tam);
uint16_t Oled_DameTamBuffer(void);

void Oled_Inicializa(void);
void Oled_Dibuja(void);
void Oled_LimpiaDisplay();
void Oled_ColocaCaracter(uint8_t *dir, const char caracter);
void Oled_EnviaCadenaXY(const char *txt, uint8_t fila, uint8_t columna);
void Oled_Finaliza(void);
