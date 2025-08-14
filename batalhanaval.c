#include <stdio.h>
#include <stdbool.h>

/*
	Jogo: Batalha Naval (nível intermediário)

	Objetivo deste programa:
	- Representar um tabuleiro 10x10 usando uma matriz bidimensional
	- Inicializar o tabuleiro com 0 (água)
	- Posicionar QUATRO navios (tamanho 3):
		1 horizontal, 1 vertical e 2 diagonais (principal e anti-diagonal)
	- Marcar as posições dos navios com o valor 3
	- Validar limites e evitar sobreposição (incluindo diagonais)
	- Imprimir o tabuleiro no console

	Simplificações:
	- Tamanho do tabuleiro fixo em 10x10
	- Tamanho dos navios fixo em 3
	- Coordenadas dos navios definidas diretamente no código
	- Sem lógica de ataques/jogadas

	Observação:
	- Todo o código está em um único arquivo C, conforme solicitado.
*/

/* Constantes do tabuleiro e valores de célula */
#define BOARD_SIZE 10
#define SHIP_SIZE 3
#define ABILITY_SIZE 5

/* Convenção de valores nas células do tabuleiro */
#define CELL_WATER 0
#define CELL_SHIP 3
#define CELL_ABILITY 5
/* Estados de jogo simples */
#define CELL_MISS 1
#define CELL_HIT 9
#define MAX_ATTEMPTS 25

/* Orientações possíveis para posicionar um navio */
typedef enum Orientation {
	HORIZONTAL = 0,
	VERTICAL = 1,
	/* Diagonal principal: avança linha+ e coluna+ (\\) */
	DIAGONAL_DOWN_RIGHT = 2,
	/* Anti-diagonal: avança linha+ e coluna- (/) */
	DIAGONAL_DOWN_LEFT = 3
} Orientation;

/* Entrada segura multiplataforma para evitar warnings no Windows */
#if defined(_WIN32)
#define SAFE_SCAN(fmt, ...) scanf_s(fmt, __VA_ARGS__)
#else
#define SAFE_SCAN(fmt, ...) scanf(fmt, __VA_ARGS__)
#endif

/* Assinaturas das funções auxiliares */
void initializeBoard(int board[BOARD_SIZE][BOARD_SIZE]);
bool isWithinBounds(int startRow, int startCol, int length, Orientation orientation);
bool doesOverlap(const int board[BOARD_SIZE][BOARD_SIZE], int startRow, int startCol, int length, Orientation orientation);
void placeShip(int board[BOARD_SIZE][BOARD_SIZE], const int ship[], int length, int startRow, int startCol, Orientation orientation);
void printBoard(const int board[BOARD_SIZE][BOARD_SIZE]);
/* Jogo simples */
void printPlayerView(const int board[BOARD_SIZE][BOARD_SIZE]);
int countRemainingShipCells(const int board[BOARD_SIZE][BOARD_SIZE]);
void playSimpleGame(int board[BOARD_SIZE][BOARD_SIZE]);
/* Assinaturas das funções de habilidades (máscaras) */
void buildConeMask(int mask[ABILITY_SIZE][ABILITY_SIZE]);
void buildCrossMask(int mask[ABILITY_SIZE][ABILITY_SIZE]);
void buildOctahedronMask(int mask[ABILITY_SIZE][ABILITY_SIZE]);
void overlayAbilityCentered(int board[BOARD_SIZE][BOARD_SIZE], int originRow, int originCol, const int mask[ABILITY_SIZE][ABILITY_SIZE]);

int main(void) {
	/* Matriz 10x10 representando o tabuleiro. 0 = água, 3 = parte do navio */
	int board[BOARD_SIZE][BOARD_SIZE];

	/* Vetores unidimensionais representando os navios (cada posição tem o valor 3) */
	int shipHorizontal[SHIP_SIZE] = { CELL_SHIP, CELL_SHIP, CELL_SHIP };
	int shipVertical[SHIP_SIZE] = { CELL_SHIP, CELL_SHIP, CELL_SHIP };
	int shipDiagonalDR[SHIP_SIZE] = { CELL_SHIP, CELL_SHIP, CELL_SHIP };
	int shipDiagonalDL[SHIP_SIZE] = { CELL_SHIP, CELL_SHIP, CELL_SHIP };

	/* Coordenadas iniciais definidas diretamente no código (linha, coluna) */
	/*
		Importante: as coordenadas abaixo foram escolhidas para serem válidas e não sobreporem.
		- Navio Horizontal (→) inicia na linha 2, coluna 1
		- Navio Vertical (↓) inicia na linha 6, coluna 5
		- Navio Diagonal Principal (↘) inicia na linha 0, coluna 7
		- Navio Anti-diagonal (↙) inicia na linha 0, coluna 2
		Observação: índices baseados em 0 (0..9)
	*/
	int startRowHorizontal = 2;
	int startColHorizontal = 1;
	int startRowVertical = 6;
	int startColVertical = 5;
	int startRowDiagDR = 0;
	int startColDiagDR = 7;
	int startRowDiagDL = 0;
	int startColDiagDL = 2;

	/* 1) Inicializa o tabuleiro inteiro com água (0) */
	initializeBoard(board);

	/* 2) Valida e posiciona o navio horizontal */
	if (!isWithinBounds(startRowHorizontal, startColHorizontal, SHIP_SIZE, HORIZONTAL)) {
		printf("Erro: navio horizontal fora dos limites do tabuleiro.\n");
		return 1;
	}
	if (doesOverlap(board, startRowHorizontal, startColHorizontal, SHIP_SIZE, HORIZONTAL)) {
		printf("Erro: navio horizontal sobrepõe outro navio.\n");
		return 1;
	}
	placeShip(board, shipHorizontal, SHIP_SIZE, startRowHorizontal, startColHorizontal, HORIZONTAL);

	/* 3) Valida e posiciona o navio vertical */
	if (!isWithinBounds(startRowVertical, startColVertical, SHIP_SIZE, VERTICAL)) {
		printf("Erro: navio vertical fora dos limites do tabuleiro.\n");
		return 1;
	}
	if (doesOverlap(board, startRowVertical, startColVertical, SHIP_SIZE, VERTICAL)) {
		printf("Erro: navio vertical sobrepõe outro navio.\n");
		return 1;
	}
	placeShip(board, shipVertical, SHIP_SIZE, startRowVertical, startColVertical, VERTICAL);

	/* 4) Valida e posiciona o navio diagonal principal (↘) */
	if (!isWithinBounds(startRowDiagDR, startColDiagDR, SHIP_SIZE, DIAGONAL_DOWN_RIGHT)) {
		printf("Erro: navio diagonal (↘) fora dos limites do tabuleiro.\n");
		return 1;
	}
	if (doesOverlap(board, startRowDiagDR, startColDiagDR, SHIP_SIZE, DIAGONAL_DOWN_RIGHT)) {
		printf("Erro: navio diagonal (↘) sobrepõe outro navio.\n");
		return 1;
	}
	placeShip(board, shipDiagonalDR, SHIP_SIZE, startRowDiagDR, startColDiagDR, DIAGONAL_DOWN_RIGHT);

	/* 5) Valida e posiciona o navio anti-diagonal (↙) */
	if (!isWithinBounds(startRowDiagDL, startColDiagDL, SHIP_SIZE, DIAGONAL_DOWN_LEFT)) {
		printf("Erro: navio anti-diagonal (↙) fora dos limites do tabuleiro.\n");
		return 1;
	}
	if (doesOverlap(board, startRowDiagDL, startColDiagDL, SHIP_SIZE, DIAGONAL_DOWN_LEFT)) {
		printf("Erro: navio anti-diagonal (↙) sobrepõe outro navio.\n");
		return 1;
	}
	placeShip(board, shipDiagonalDL, SHIP_SIZE, startRowDiagDL, startColDiagDL, DIAGONAL_DOWN_LEFT);

	/* 6) Imprime o tabuleiro resultante */
	printBoard(board);

	/* ==========================
	   HABILIDADES (ÁREAS DE EFEITO)
	   ==========================
		- Construímos três máscaras 5x5 usando loops e condicionais:
		  CONE (apontando para baixo, com ápice no centro da máscara), CRUZ (centrada) e OCTAEDRO (losango, centrado).
		- Sobrepomos cada máscara ao tabuleiro, centralizando no ponto de origem.
	*/

	int maskCone[ABILITY_SIZE][ABILITY_SIZE];
	int maskCross[ABILITY_SIZE][ABILITY_SIZE];
	int maskOcta[ABILITY_SIZE][ABILITY_SIZE];

	buildConeMask(maskCone);
	buildCrossMask(maskCross);
	buildOctahedronMask(maskOcta);

	/* Pontos de origem para cada habilidade (centrados) */
	int originConeRow = 2, originConeCol = 2;     /* cone abaixo do centro (2,2) */
	int originCrossRow = 7, originCrossCol = 2;   /* cruz próxima da parte inferior esquerda */
	int originOctaRow = 5, originOctaCol = 7;    /* octaedro à direita */

	/* Sobrepõe as áreas de efeito no tabuleiro (valor 5) */
	overlayAbilityCentered(board, originConeRow, originConeCol, maskCone);
	overlayAbilityCentered(board, originCrossRow, originCrossCol, maskCross);
	overlayAbilityCentered(board, originOctaRow, originOctaCol, maskOcta);

	/* Imprime o tabuleiro com as áreas de habilidade aplicadas */
	printf("\n");
	printBoard(board);

	/* ==========================
	   MODO DE JOGO (SIMPLES)
	   ==========================
		- O jogador informa coordenadas (linha e coluna) de 0 a 9
		- Acerto (em navio 3) vira 9
		- Erro (água 0 ou habilidade 5) vira 1
		- O jogo termina ao afundar todos os navios ou atingir o limite de tentativas
	*/
	playSimpleGame(board);

	return 0;
}

/*
	initializeBoard
	Inicializa todo o tabuleiro com água (valor 0).
*/
void initializeBoard(int board[BOARD_SIZE][BOARD_SIZE]) {
	for (int row = 0; row < BOARD_SIZE; row++) {
		for (int col = 0; col < BOARD_SIZE; col++) {
			board[row][col] = CELL_WATER;
		}
	}
}

/*
	isWithinBounds
	Verifica se, a partir de uma coordenada inicial (linha/coluna), um navio de
	"length" posições cabe no tabuleiro na orientação indicada.
*/
bool isWithinBounds(int startRow, int startCol, int length, Orientation orientation) {
	/* Índices base sempre precisam estar no tabuleiro */
	if (startRow < 0 || startRow >= BOARD_SIZE || startCol < 0 || startCol >= BOARD_SIZE) {
		return false;
	}

	if (orientation == HORIZONTAL) {
		/* Cabe horizontalmente: colunas startCol até startCol + length - 1 */
		return (startCol + length - 1) < BOARD_SIZE;
	}

	if (orientation == VERTICAL) {
		/* Cabe verticalmente: linhas startRow até startRow + length - 1 */
		return (startRow + length - 1) < BOARD_SIZE;
	}

	if (orientation == DIAGONAL_DOWN_RIGHT) {
		/* Diagonal principal: (r+i, c+i) deve permanecer no tabuleiro */
		return (startRow + length - 1) < BOARD_SIZE && (startCol + length - 1) < BOARD_SIZE;
	}

	if (orientation == DIAGONAL_DOWN_LEFT) {
		/* Anti-diagonal: (r+i, c-i) deve permanecer no tabuleiro */
		return (startRow + length - 1) < BOARD_SIZE && (startCol - (length - 1)) >= 0;
	}

	return false;
}

/*
	doesOverlap
	Verifica se alguma das posições onde o navio seria colocado já está ocupada
	por outro navio (valor 3).
*/
bool doesOverlap(const int board[BOARD_SIZE][BOARD_SIZE], int startRow, int startCol, int length, Orientation orientation) {
	if (orientation == HORIZONTAL) {
		for (int offset = 0; offset < length; offset++) {
			if (board[startRow][startCol + offset] != CELL_WATER) {
				return true;
			}
		}
		return false;
	}

	if (orientation == VERTICAL) {
		for (int offset = 0; offset < length; offset++) {
			if (board[startRow + offset][startCol] != CELL_WATER) {
				return true;
			}
		}
		return false;
	}

	if (orientation == DIAGONAL_DOWN_RIGHT) {
		for (int offset = 0; offset < length; offset++) {
			if (board[startRow + offset][startCol + offset] != CELL_WATER) {
				return true;
			}
		}
		return false;
	}

	if (orientation == DIAGONAL_DOWN_LEFT) {
		for (int offset = 0; offset < length; offset++) {
			if (board[startRow + offset][startCol - offset] != CELL_WATER) {
				return true;
			}
		}
		return false;
	}

	return true; /* orientação desconhecida: trate como sobreposição */
}

/*
	placeShip
	Copia os valores do vetor do navio (3,3,3) para as posições adequadas do
	tabuleiro, de acordo com a orientação e coordenadas iniciais.
*/
void placeShip(int board[BOARD_SIZE][BOARD_SIZE], const int ship[], int length, int startRow, int startCol, Orientation orientation) {
	if (orientation == HORIZONTAL) {
		for (int offset = 0; offset < length; offset++) {
			board[startRow][startCol + offset] = ship[offset];
		}
		return;
	}

	if (orientation == VERTICAL) {
		for (int offset = 0; offset < length; offset++) {
			board[startRow + offset][startCol] = ship[offset];
		}
		return;
	}

	if (orientation == DIAGONAL_DOWN_RIGHT) {
		for (int offset = 0; offset < length; offset++) {
			board[startRow + offset][startCol + offset] = ship[offset];
		}
		return;
	}

	if (orientation == DIAGONAL_DOWN_LEFT) {
		for (int offset = 0; offset < length; offset++) {
			board[startRow + offset][startCol - offset] = ship[offset];
		}
		return;
	}
}

/*
	printBoard
	Imprime a matriz completa, separando elementos por espaço para facilitar a
	visualização. Opcionalmente imprime cabeçalhos de linha/coluna para clareza.
*/
void printBoard(const int board[BOARD_SIZE][BOARD_SIZE]) {
	/* Cabeçalho de colunas */
	printf("    ");
	for (int col = 0; col < BOARD_SIZE; col++) {
		printf("%2d ", col);
	}
	printf("\n");

	printf("   ");
	for (int col = 0; col < BOARD_SIZE; col++) {
		printf("---");
	}
	printf("\n");

	for (int row = 0; row < BOARD_SIZE; row++) {
		/* Índice da linha no início */
		printf("%2d| ", row);
		for (int col = 0; col < BOARD_SIZE; col++) {
			printf("%2d ", board[row][col]);
		}
		printf("\n");
	}
}


/* ==========================
	CONSTRUÇÃO DAS MÁSCARAS DE HABILIDADE (DINÂMICAS)
   ========================== */

/*
	buildConeMask
	Gera uma máscara triangular ("cone" apontando para baixo) de tamanho size x size.
	- O ápice fica na linha 0 e coluna central (mid)
	- A cada linha r, a largura cresce 1 para cada lado: colunas [mid - r, mid + r]
*/
void buildConeMask(int mask[ABILITY_SIZE][ABILITY_SIZE]) {
	int mid = ABILITY_SIZE / 2;
	for (int r = 0; r < ABILITY_SIZE; r++) {
		for (int c = 0; c < ABILITY_SIZE; c++) {
			mask[r][c] = 0;
		}
	}
	for (int r = 0; r < ABILITY_SIZE; r++) {
		int left = mid - r;
		int right = mid + r;
		if (left < 0) left = 0;
		if (right >= ABILITY_SIZE) right = ABILITY_SIZE - 1;
		for (int c = left; c <= right; c++) {
			mask[r][c] = 1;
		}
	}
}

/*
	buildCrossMask
	Gera uma máscara em forma de cruz centrada.
	- Marca a linha central e a coluna central com 1
*/
void buildCrossMask(int mask[ABILITY_SIZE][ABILITY_SIZE]) {
	int mid = ABILITY_SIZE / 2;
	for (int r = 0; r < ABILITY_SIZE; r++) {
		for (int c = 0; c < ABILITY_SIZE; c++) {
			mask[r][c] = (r == mid || c == mid) ? 1 : 0;
		}
	}
}

/*
	buildOctahedronMask
	Gera uma máscara em formato losango (vista frontal de um octaedro).
	- Usa distância de Manhattan ao centro: |dr| + |dc| <= mid
*/
void buildOctahedronMask(int mask[ABILITY_SIZE][ABILITY_SIZE]) {
	int mid = ABILITY_SIZE / 2;
	for (int r = 0; r < ABILITY_SIZE; r++) {
		for (int c = 0; c < ABILITY_SIZE; c++) {
			int dr = r - mid; if (dr < 0) dr = -dr;
			int dc = c - mid; if (dc < 0) dc = -dc;
			mask[r][c] = (dr + dc <= mid) ? 1 : 0;
		}
	}
}

/*
	overlayAbilityCentered
	Sobrepõe uma máscara size x size no tabuleiro, centralizando no ponto de origem.
	- Células com 1 na máscara viram 5 no tabuleiro, preservando navios (3)
	- Ignora posições fora do tabuleiro
*/
void overlayAbilityCentered(int board[BOARD_SIZE][BOARD_SIZE], int originRow, int originCol, const int mask[ABILITY_SIZE][ABILITY_SIZE]) {
	int mid = ABILITY_SIZE / 2;
	for (int r = 0; r < ABILITY_SIZE; r++) {
		for (int c = 0; c < ABILITY_SIZE; c++) {
			if (mask[r][c] == 1) {
				int br = originRow + (r - mid);
				int bc = originCol + (c - mid);
				if (br >= 0 && br < BOARD_SIZE && bc >= 0 && bc < BOARD_SIZE) {
					if (board[br][bc] == CELL_WATER) {
						board[br][bc] = CELL_ABILITY;
					}
				}
			}
		}
	}
}

/* 2 4
4 2
MECÂNICA DE JOGO (SIMPLES) */

/*
	printPlayerView
	Exibe a visão do jogador, ocultando navios intactos e habilidades.
	- Mostra H (9) para acerto e . (1) para erro.
	- Mostra ~ para água desconhecida; navios não atingidos e habilidades ficam ocultos como ~.
*/
void printPlayerView(const int board[BOARD_SIZE][BOARD_SIZE]) {
	printf("\nVisao do Jogador:\n");
	printf("    ");
	for (int col = 0; col < BOARD_SIZE; col++) printf("%2d ", col);
	printf("\n   ");
	for (int col = 0; col < BOARD_SIZE; col++) printf("---");
	printf("\n");
	for (int row = 0; row < BOARD_SIZE; row++) {
		printf("%2d| ", row);
		for (int col = 0; col < BOARD_SIZE; col++) {
			int v = board[row][col];
			char ch = '~';
			if (v == CELL_HIT) ch = 'H';
			else if (v == CELL_MISS) ch = '.';
			printf(" %c ", ch);
		}
		printf("\n");
	}
}

/*
	countRemainingShipCells
	Conta quantas células de navio (3) ainda restam não atingidas.
*/
int countRemainingShipCells(const int board[BOARD_SIZE][BOARD_SIZE]) {
	int remaining = 0;
	for (int r = 0; r < BOARD_SIZE; r++) {
		for (int c = 0; c < BOARD_SIZE; c++) {
			if (board[r][c] == CELL_SHIP) remaining++;
		}
	}
	return remaining;
}

/*
	playSimpleGame
	Loop simples de jogadas:
	- Lê linha e coluna (0..9)
	- Marca acerto (H=9) se houver navio (3), caso contrário erro (.=1) se for água/área (0/5)
	- Ignora repetição de jogada na mesma casa
*/
void playSimpleGame(int board[BOARD_SIZE][BOARD_SIZE]) {
	int attempts = 0;
	int remaining = countRemainingShipCells(board);
	printf("\nJogo iniciado! Restam %d partes de navios. Max tentativas: %d\n", remaining, MAX_ATTEMPTS);
	printPlayerView(board);

	while (remaining > 0 && attempts < MAX_ATTEMPTS) {
		int row = -1, col = -1;
		printf("\nDigite linha e coluna (0..9), separados por espaco: ");
		int read = SAFE_SCAN("%d %d", &row, &col);
		if (read != 2) {
			/* Limpa entrada inválida */
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF) {}
			printf("Entrada invalida. Tente novamente.\n");
			continue;
		}
		if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
			printf("Coordenadas fora do limite.\n");
			continue;
		}

		int cell = board[row][col];
		if (cell == CELL_HIT || cell == CELL_MISS) {
			printf("Posicao ja jogada.\n");
			continue;
		}

		attempts++;
		if (cell == CELL_SHIP) {
			board[row][col] = CELL_HIT;
			remaining--;
			printf("Acerto! Restam %d.\n", remaining);
		} else {
			/* Considera erro sobre água (0) e habilidade (5) */
			board[row][col] = CELL_MISS;
			printf("Agua!\n");
		}

		printPlayerView(board);
	}

	if (remaining == 0) {
		printf("\nParabens! Voce afundou todos os navios em %d jogadas.\n", attempts);
	} else {
		printf("\nFim de jogo! Tentativas esgotadas. Restaram %d partes de navios.\n", remaining);
	}
}


