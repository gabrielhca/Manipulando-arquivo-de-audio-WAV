#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#pragma pack(1)

// Estrutura do cabeçalho WAV usando tipos inteiros
typedef struct {
    char ChunkID[4];          // "RIFF"
    int ChunkSize;            // Tamanho do arquivo - 8 bytes
    char Format[4];           // "WAVE"
    char SubChunk1ID[4];      // "fmt "
    int SubChunk1Size;        // 16 para PCM
    short AudioFormat;        // 1 para PCM
    short NumChannels;        // Número de canais
    int SampleRate;           // Taxa de amostragem
    int ByteRate;             // Taxa de bytes por segundo
    short BlockAlign;         // Número de bytes por amostra
    short BitsPerSample;      // Bits por amostra
    char SubChunk2ID[4];      // "data"
    int SubChunk2Size;        // Número de bytes de dados de áudio
} WAVHeader;

// Imprime o cabeçalho WAV
void imprimeHeader(WAVHeader *header) {
    printf("ChunkID: %.4s\n", header->ChunkID);
    printf("ChunkSize: %d\n", header->ChunkSize);
    printf("Format: %.4s\n", header->Format);
    printf("SubChunk1ID: %.4s\n", header->SubChunk1ID);
    printf("SubChunk1Size: %d\n", header->SubChunk1Size);
    printf("AudioFormat: %d\n", header->AudioFormat);
    printf("NumChannels: %d\n", header->NumChannels);
    printf("SampleRate: %d\n", header->SampleRate);
    printf("ByteRate: %d\n", header->ByteRate);
    printf("BlockAlign: %d\n", header->BlockAlign);
    printf("BitsPerSample: %d\n", header->BitsPerSample);
    printf("SubChunk2ID: %.4s\n", header->SubChunk2ID);
    printf("SubChunk2Size: %d\n", header->SubChunk2Size);
}

// Copia um arquivo WAV
void arquivoCopia(const char *entrada, const char *saida) {
    FILE *in = fopen(entrada, "rb");
    FILE *out = fopen(saida, "wb");
    if (!in || !out) {
        perror("Erro ao abrir arquivos");
        return;
    }
    WAVHeader header;
    fread(&header, sizeof(WAVHeader), 1, in);
    fwrite(&header, sizeof(WAVHeader), 1, out);
    short amostra;
    while (fread(&amostra, sizeof(short), 1, in)) {
        fwrite(&amostra, sizeof(short), 1, out);
    }
    fclose(in);
    fclose(out);
}

// Encontra o valor máximo e mínimo da amostra
void amostraMaxMin(const char *nomeArquivo) {
    FILE *file = fopen(nomeArquivo, "rb");
    if (!file) {
        perror("Erro ao abrir arquivo");
        return;
    }

    WAVHeader header;
    if (fread(&header, sizeof(WAVHeader), 1, file) != 1) {
        fprintf(stderr, "Erro ao ler o cabeçalho do arquivo WAV.\n");
        fclose(file);
        return;
    }

    if (strncmp(header.ChunkID, "RIFF", 4) != 0 || strncmp(header.Format, "WAVE", 4) != 0) {
        printf("Não é um arquivo WAV válido.\n");
        fclose(file);
        return;
    }

    short minAmostra = SHRT_MAX;
    short maxAmostra = SHRT_MIN;
    short amostra;
    int bytesPorAmostra = header.BitsPerSample / 8;
    int numAmostras = header.SubChunk2Size / (header.NumChannels * bytesPorAmostra);

    // Posiciona o ponteiro no início dos dados de áudio
    fseek(file, 44, SEEK_SET);

    for (int i = 0; i < numAmostras; i++) {
        if (fread(&amostra, bytesPorAmostra, 1, file) != 1) {
            fprintf(stderr, "Erro ao ler amostra %d\n", i);
            fclose(file);
            return;
        }

        // Verifica se os canais estão entrelaçados e pula para o próximo canal, se necessário
        if (header.NumChannels > 1) {
            fseek(file, (header.NumChannels - 1) * bytesPorAmostra, SEEK_CUR);
        }

        if (amostra < minAmostra) minAmostra = amostra;
        if (amostra > maxAmostra) maxAmostra = amostra;
    }

    printf("Valor mínimo da amostra: %d\n", minAmostra);
    printf("Valor máximo da amostra: %d\n", maxAmostra);
    fclose(file);
}

int main() {
    int tarefa;
    do {
        printf("\nMENU\n1 - Imprimir Cabeçalho\n2 - Copiar WAV\n3 - Max/Min Amostra\n0 - Sair\nEscolha: ");
        scanf("%d", &tarefa);
        switch (tarefa) {
            case 0: printf("Fim!\n"); break;
            case 1: {
                FILE *file = fopen("darkness-code.wav", "rb");
                if (!file) { perror("Erro ao abrir"); return 1; }
                WAVHeader header;
                fread(&header, sizeof(WAVHeader), 1, file);
                if (strncmp(header.ChunkID, "RIFF", 4) != 0 || strncmp(header.Format, "WAVE", 4) != 0) {
                    printf("Não é WAV válido.\n");
                    fclose(file);
                    return 1;
                }
                imprimeHeader(&header);
                fclose(file);
                break;
            }
            case 2: arquivoCopia("darkness-code.wav", "audio_copia.wav"); break;
            case 3: amostraMaxMin("darkness-code.wav"); break;
            default: printf("Tarefa inválida.\n"); break;
        }
    } while (tarefa != 0);
    return 0;
}
