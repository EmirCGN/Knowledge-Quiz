#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SCORE_FILE "score.txt"
#define QUESTIONS_FILE "D:/m/Intern/Development/EBR/fragen.txt"
#define MAX_FRAGEN 100
#define MAX_NAME 50

typedef struct {
    char frage[256];
    char antworten[4][128];
    int richtigeAntwort;
} Frage;

// Funktion zum Mischen der Fragen
void mischeFragen(Frage fragen[], int anzahl) {
    srand(time(NULL));
    for (int i = anzahl - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Frage temp = fragen[i];
        fragen[i] = fragen[j];
        fragen[j] = temp;
    }
}

// Funktion zum Laden des Punktestands
int ladePunkte() {
    FILE *file = fopen(SCORE_FILE, "r");
    if (file == NULL) {
        return 0;
    }
    int punkte;
    fscanf(file, "%d", &punkte);
    fclose(file);
    return punkte;
}

// Funktion zum Speichern der Punkte mit Zeitstempel
void speicherePunkte(int punkte, char *name) {
    FILE *file = fopen(SCORE_FILE, "r");
    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("Fehler beim Öffnen der Datei zum Speichern!\n");
        return;
    }
    char line[256];
    int nameGefunden = 0;
    time_t t;
    time(&t);

    while (file && fgets(line, sizeof(line), file)) {
        char gespeicherterName[MAX_NAME];
        int gespeichertePunkte;

        if (sscanf(line, "%s - %d Punkte", gespeicherterName, &gespeichertePunkte) == 2) {
            if (strcmp(gespeicherterName, name) == 0) {
                fprintf(tempFile, "%s - %d Punkte (aktualisiert am: %s)", name, punkte, ctime(&t));
                nameGefunden = 1;
                continue;
            }
        }
        fprintf(tempFile, "%s", line);
    }
    if (!nameGefunden) {
        fprintf(tempFile, "%s - %d Punkte (erstellt am: %s)", name, punkte, ctime(&t));
    }

    fclose(file);
    fclose(tempFile);

    // Ersetze alte Datei mit der neuen
    remove(SCORE_FILE);
    rename("temp.txt", SCORE_FILE);
}

// Funktion zum Anzeigen der Statistiken
void zeigeStatistiken() {
    FILE *file = fopen(SCORE_FILE, "r");
    if (file == NULL) {
        printf("Keine gespeicherten Statistiken gefunden.\n");
        return;
    }
    char line[256];
    printf("\n--- Statistiken ---\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

// Funktion zum Hinzufügen neuer Fragen
void frageHinzufuegen() {
    FILE *file = fopen(QUESTIONS_FILE, "a");
    if (file == NULL) {
        printf("Fehler: Datei konnte nicht geöffnet werden.\n");
        return;
    }

    char frage[256], antworten[4][128];
    int richtigeAntwort;

    printf("Gib die neue Frage ein: ");
    getchar(); // Pufferspeicher leeren
    fgets(frage, sizeof(frage), stdin);
    frage[strcspn(frage, "\n")] = 0;  // Entfernt \n

    for (int i = 0; i < 4; i++) {
        printf("Antwort %d: ", i + 1);
        fgets(antworten[i], sizeof(antworten[i]), stdin);
        antworten[i][strcspn(antworten[i], "\n")] = 0;
    }

    printf("Welche Antwort ist richtig? (1-4): ");
    scanf("%d", &richtigeAntwort);

    fprintf(file, "%s\n%s\n%s\n%s\n%s\n%d\n",
            frage, antworten[0], antworten[1], antworten[2], antworten[3], richtigeAntwort);

    fclose(file);
    printf("Frage erfolgreich hinzugefügt!\n");
}

// Funktion zum Laden der Fragen aus der Datei
int ladeFragen(Frage fragen[], int maxFragen) {
    FILE *file = fopen(QUESTIONS_FILE, "r");
    if (file == NULL) {
        printf("Fehler: Datei mit Fragen konnte nicht geladen werden.\n");
        return 0;
    }

    int count = 0;
    char buffer[512];

    while (count < maxFragen && fgets(fragen[count].frage, sizeof(fragen[count].frage), file)) {
        fragen[count].frage[strcspn(fragen[count].frage, "\n")] = 0;

        for (int i = 0; i < 4; i++) {
            if (!fgets(fragen[count].antworten[i], sizeof(fragen[count].antworten[i]), file)) {
                fclose(file);
                return count;
            }
            fragen[count].antworten[i][strcspn(fragen[count].antworten[i], "\n")] = 0;
        }

        if (fgets(buffer, sizeof(buffer), file)) {
            sscanf(buffer, "%d", &fragen[count].richtigeAntwort);
        } else {
            fclose(file);
            return count;
        }

        count++;
    }

    fclose(file);
    return count;
}

// Funktion zum Stellen einer Frage
int stelleFrage(Frage q) {
    printf("\n%s\n", q.frage);
    for (int i = 0; i < 4; i++) {
        printf("%d) %s\n", i + 1, q.antworten[i]);
    }
    printf("Deine Auswahl (1-4): ");

    int antwort;
    while (1) {
        if (scanf("%d", &antwort) == 1 && antwort >= 1 && antwort <= 4) {
            break;
        } else {
            printf("Ungültige Eingabe! Bitte eine Zahl zwischen 1 und 4 eingeben: ");
            while (getchar() != '\n');
        }
    }
    return antwort;
}

// Funktion zum Quiz starten
void quizStarten(char *name) {
    Frage fragen[MAX_FRAGEN];
    int anzahlFragen = ladeFragen(fragen, MAX_FRAGEN);
    if (anzahlFragen == 0) {
        printf("Keine Fragen geladen. Quiz kann nicht gestartet werden.\n");
        return;
    }

    mischeFragen(fragen, anzahlFragen);
    int punkte = 0;

    int richtige = 0;
    for (int i = 0; i < anzahlFragen; i++) {
        int antwort = stelleFrage(fragen[i]);
        if (antwort == fragen[i].richtigeAntwort) {
            printf("✅ Richtig!\n");
            punkte += 10;
            richtige++;
        } else {
            printf("❌ Falsch! Die richtige Antwort war: %s\n", fragen[i].antworten[fragen[i].richtigeAntwort - 1]);
            punkte -= 5;
        }
    }

    printf("\nDu hast %d von %d Fragen richtig beantwortet. Neuer Punktestand: %d Punkte\n", richtige, anzahlFragen, punkte);
    speicherePunkte(punkte, name);
}

void menue() {
    int auswahl;
    char name[MAX_NAME];

    printf("Bitte gebe  deinen Namen ein: ");
    scanf("%s", name);

    do {
        printf("\n--- Quiz-Menü ---\n");
        printf("1) Quiz starten\n");
        printf("2) Statistiken anzeigen\n");
        printf("3) Neue Fragen hinzufügen\n");
        printf("4) Programm beenden\n");
        scanf("%d", &auswahl);

        switch (auswahl) {
            case 1:
            quizStarten(name);
            break;
            case 2:
                zeigeStatistiken();
                break;
            case 3:
                frageHinzufuegen();
            break;
            case 4:
                printf("Program wird beendet...\n");
            break;
            default:
                printf("Ungültige Option! Bitte erneut eingeben.\n");
        }
    }while (auswahl !=4);
}

int main() {
    menue();
    return 0;
}
