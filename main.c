#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SCORE_FILE "score.txt"
#define QUESTIONS_FILE "fragen.txt"
#define MAX_FRAGEN 100
#define MAX_NAME 50
#define MAX_SPIELER 1000

typedef struct {
    char frage[256];
    char antworten[4][128];
    int richtigeAntwort;
} Frage;

typedef struct {
    char name[MAX_NAME];
    int punkte;
} Spieler;

Spieler spielerListe[MAX_SPIELER];
int spielerAnzahl = 0;

// 🔹 Verbesserte Shuffle-Methode mit Durstenfeld-Fisher-Yates
void mischeFragen(Frage fragen[], int anzahl) {
    for (int i = 0; i < anzahl - 1; i++) {
        int j = i + rand() % (anzahl - i);
        Frage temp = fragen[i];
        fragen[i] = fragen[j];
        fragen[j] = temp;
    }
}

// 🔹 Punkte speichern (doppelte Namen überschreiben)
void speicherePunkte(char *name, int punkte) {
    int gefunden = 0;
    for (int i = 0; i < spielerAnzahl; i++) {
        if (strcmp(spielerListe[i].name, name) == 0) {
            spielerListe[i].punkte = punkte;
            gefunden = 1;
            break;
        }
    }
    if (!gefunden && spielerAnzahl < MAX_SPIELER) {
        strcpy(spielerListe[spielerAnzahl].name, name);
        spielerListe[spielerAnzahl].punkte = punkte;
        spielerAnzahl++;
    }

    FILE *file = fopen(SCORE_FILE, "w");
    for (int i = 0; i < spielerAnzahl; i++) {
        fprintf(file, "%s - %d Punkte\n", spielerListe[i].name, spielerListe[i].punkte);
    }
    fclose(file);
}

// 🔹 Highscore mit QuickSort
void quickSort(Spieler arr[], int low, int high) {
    if (low < high) {
        int pivot = arr[high].punkte;
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (arr[j].punkte > pivot) {
                i++;
                Spieler temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
        Spieler temp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = temp;
        quickSort(arr, low, i);
        quickSort(arr, i + 2, high);
    }
}

// 🔹 Highscore anzeigen
void zeigeHighscore() {
    quickSort(spielerListe, 0, spielerAnzahl - 1);
    printf("\n--- Highscores ---\n");
    for (int i = 0; i < spielerAnzahl && i < 10; i++) {
        printf("%d. %s - %d Punkte\n", i + 1, spielerListe[i].name, spielerListe[i].punkte);
    }
}

// 🔹 Fragen hinzufügen mit Fehlerüberprüfung
void frageHinzufuegen() {
    FILE *file = fopen(QUESTIONS_FILE, "a");
    if (file == NULL) {
        printf("Fehler: Datei konnte nicht geöffnet werden.\n");
        return;
    }

    char frage[256], antworten[4][128];
    int richtigeAntwort;

    printf("Gib die neue Frage ein: ");
    getchar();
    fgets(frage, sizeof(frage), stdin);
    frage[strcspn(frage, "\n")] = 0;

    for (int i = 0; i < 4; i++) {
        printf("Antwort %d: ", i + 1);
        fgets(antworten[i], sizeof(antworten[i]), stdin);
        antworten[i][strcspn(antworten[i], "\n")] = 0;
    }

    printf("Welche Antwort ist richtig? (1-4): ");
    while (scanf("%d", &richtigeAntwort) != 1 || richtigeAntwort < 1 || richtigeAntwort > 4) {
        printf("Ungültige Eingabe! Bitte eine Zahl zwischen 1 und 4 eingeben: ");
        while (getchar() != '\n');
    }

    fprintf(file, "%s\n%s\n%s\n%s\n%s\n%d\n",
            frage, antworten[0], antworten[1], antworten[2], antworten[3], richtigeAntwort);
    fclose(file);
    printf("Frage erfolgreich hinzugefügt!\n");
}

// 🔹 Quiz starten
void quizStarten(char *name) {
    Frage fragen[MAX_FRAGEN];
    int anzahlFragen = 10;
    mischeFragen(fragen, anzahlFragen);
    int punkte = 0;

    for (int i = 0; i < anzahlFragen; i++) {
        printf("\n%s\n", fragen[i].frage);
        for (int j = 0; j < 4; j++) {
            printf("%d) %s\n", j + 1, fragen[i].antworten[j]);
        }
        int antwort;
        printf("Deine Auswahl: ");
        while (scanf("%d", &antwort) != 1 || antwort < 1 || antwort > 4) {
            printf("Ungültige Eingabe! Bitte eine Zahl zwischen 1 und 4 eingeben: ");
            while (getchar() != '\n');
        }

        if (antwort == fragen[i].richtigeAntwort) {
            printf("✅ Richtig!\n");
            punkte += 10;
        } else {
            printf("❌ Falsch! Die richtige Antwort war: %s\n", fragen[i].antworten[fragen[i].richtigeAntwort - 1]);
            punkte -= 5;
        }
    }
    printf("\nNeuer Punktestand: %d Punkte\n", punkte);
    speicherePunkte(name, punkte);
}

// 🔹 Menü für das Quiz
void menue() {
    char name[MAX_NAME];
    printf("Bitte gebe deinen Namen ein: ");
    scanf("%s", name);

    int auswahl;
    do {
        printf("\n--- Quiz-Menue ---\n");
        printf("1) Quiz starten\n");
        printf("2) Highscore anzeigen\n");
        printf("3) Neue Fragen hinzufügen\n");
        printf("4) Programm beenden\n");
        printf("Auswahl: ");
        while (scanf("%d", &auswahl) != 1 || auswahl < 1 || auswahl > 4) {
            printf("Ungültige Auswahl! Bitte 1-4 eingeben: ");
            while (getchar() != '\n');
        }

        switch (auswahl) {
            case 1:
                quizStarten(name);
            break;
            case 2:
                zeigeHighscore();
            break;
            case 3:
                frageHinzufuegen();
            break;
            case 4:
                printf("Programm wird beendet...\n");
            break;
        }
    } while (auswahl != 4);
}

// 🔹 Hauptfunktion
int main() {
    srand(time(NULL));
    menue();
    return 0;
}
