#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

typedef struct
{
    char name[25];
    int size;
} Game;

Game library[25];           // Stores available games
Game installedGames[20];    // Stores installed games

int numberOfLibraryGames = 0;   // Number of games in library
int numberOfInstalledGames = 0; // Number of installed games
int usedMemory = 0;             // Used memory in GB

void set_color(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void clear_screen()
{
    system(CLEAR_SCREEN);
}

void pause_console()
{
    printf("\nPress ENTER to continue...");
    getchar();
    getchar();
}

void display_games(Game games[], int n)
{
    if (n == 0)
    {
        printf("No games found!\n");
        return;
    }

    for (int i = 0; i < n; i++)
    {
        printf("%d. ", i + 1);

        set_color(11); // Cyan
        printf("%s", games[i].name);

        set_color(7); // Reset to default
        printf(" - ");

        set_color(6); // Yellow
        printf("%d ", games[i].size);

        set_color(7); // Reset to default

        printf("GB\n");
    }
}

void display_menu()
{
    clear_screen();
    printf("\n======================================\n");
    printf("         GAME LIBRARY MANAGER      \n");
    printf("======================================\n");

    printf("\nMenu Options:\n");
    printf("--------------------------------------\n");
    printf("1. Display Installed Games\n");
    printf("2. Display Games in Library\n");
    printf("3. Install a Game\n");
    printf("4. Uninstall a Game\n");
    printf("5. Exit\n");
    printf("--------------------------------------\n");
}
void install_game(char *name)
{
    for (int i = 0; i < numberOfLibraryGames; i++)
    {
        if (strcmp(library[i].name, name) == 0)
        {
            if (usedMemory + library[i].size <= 1024)
            {
                installedGames[numberOfInstalledGames++] = library[i];
                usedMemory += library[i].size;
                printf("\n %s has been installed!\n", name);

                FILE *outfile = fopen("installed_games.txt", "w");
                for (int j = 0; j < numberOfInstalledGames; j++)
                {
                    fprintf(outfile, "%s,%d\n", installedGames[j].name, installedGames[j].size);
                }
                fclose(outfile);
            }
            else
            {
                printf(" Not enough storage!\n");
            }
            return;
        }
    }
    printf(" Game not found in library!\n");
}

void uninstall_game(char *name)
{
    for (int i = 0; i < numberOfInstalledGames; i++)
    {
        if (strcmp(installedGames[i].name, name) == 0)
        {
            usedMemory -= installedGames[i].size;
            for (int j = i; j < numberOfInstalledGames - 1; j++)
            {
                installedGames[j] = installedGames[j + 1];
            }
            numberOfInstalledGames--;
            printf("\n The game has been uninstalled.\n");

            FILE *outfile = fopen("installed_games.txt", "w");
            for (int j = 0; j < numberOfInstalledGames; j++)
            {
                fprintf(outfile, "%s,%d\n", installedGames[j].name, installedGames[j].size);
            }
            fclose(outfile);

            return;
        }
    }
    printf(" Game not found in installed list!\n");
}

void sort_games(int choice)
{
    if (numberOfInstalledGames <= 1)
    {
        printf("Not enough games to sort!\n");
        return;
    }

    for (int i = 0; i < numberOfInstalledGames - 1; i++)
    {
        for (int j = i + 1; j < numberOfInstalledGames; j++)
        {
            if ((choice == 1 && strcmp(installedGames[i].name, installedGames[j].name) > 0) ||
                    (choice == 2 && installedGames[i].size > installedGames[j].size))
            {
                Game tmp = installedGames[i];
                installedGames[i] = installedGames[j];
                installedGames[j] = tmp;
            }
        }
    }

    clear_screen();
    printf("Sorted Installed Games\n============================\n");
    display_games(installedGames, numberOfInstalledGames);
}

void load_games_from_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf(" Error: Could not open file %s\n", filename);
        return;
    }

    while (fscanf(file, "%24[^,],%d\n", library[numberOfLibraryGames].name, &library[numberOfLibraryGames].size) == 2)
    {
        numberOfLibraryGames++;
        if (numberOfLibraryGames >= 25) break;
    }
    fclose(file);
}

void load_installed_games_from_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf(" No previously installed games found.\n");
        return;
    }

    numberOfInstalledGames = 0;
    usedMemory = 0;

    while (fscanf(file, "%24[^,],%d\n", installedGames[numberOfInstalledGames].name, &installedGames[numberOfInstalledGames].size) == 2)
    {
        usedMemory += installedGames[numberOfInstalledGames].size;
        numberOfInstalledGames++;
        if (numberOfInstalledGames >= 20) break;
    }

    fclose(file);
}

void handle_display_installed_games()
{
    printf("Installed Games\n============================\n");
    display_games(installedGames, numberOfInstalledGames);

    printf("\n--------------------------------------\n");
    printf("Used Storage: %d GB / 1024 GB\n", usedMemory);
    printf("--------------------------------------\n");

    if (numberOfInstalledGames > 1)
    {
        int sort_choice;
        printf("\nSort Options:\n");
        printf("1. Name (A-Z)\n");
        printf("2. Size (Smallest to Largest)\n");
        printf("3. Go Back\n");
        printf("Choose sorting method: ");
        scanf("%d", &sort_choice);
        if (sort_choice == 1 || sort_choice == 2)
        {
            sort_games(sort_choice);
        }
        else
        {
            printf("\nReturning to main menu...\n");
        }
    }
}

void handle_display_library_games()
{
    printf("Available Games\n============================\n");
    display_games(library, numberOfLibraryGames);
}

void handle_install_game()
{
    char game_name[25];
    getchar(); // Clear input buffer
    printf("Enter a game to install: ");
    fgets(game_name, sizeof(game_name), stdin);
    game_name[strcspn(game_name, "\n")] = 0; // Remove newline
    install_game(game_name);
}

void handle_uninstall_game()
{
    char game_name[25];
    getchar(); // Clear input buffer
    printf("Enter a game to uninstall: ");
    fgets(game_name, sizeof(game_name), stdin);
    game_name[strcspn(game_name, "\n")] = 0; // Remove newline
    uninstall_game(game_name);
}

void handle_menu_choice(int choice)
{
    clear_screen();
    switch (choice)
    {
    case 1:
        handle_display_installed_games();
        break;
    case 2:
        handle_display_library_games();
        break;
    case 3:
        handle_install_game();
        break;
    case 4:
        handle_uninstall_game();
        break;
    case 5:
        printf("Exiting the Game Library Manager. Goodbye!\n");
        exit(0);
    default:
        printf("\nInvalid choice! Please select a valid option (1-5).\n");
    }
}

void run_program()
{
    int choice = -1;
    while (1)
    {
        display_menu();
        printf("Select an option (1-5): ");
        if (scanf("%d", &choice) != 1)
        {
            printf("\nInvalid input! Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            pause_console();
            continue;
        }

        handle_menu_choice(choice);
        pause_console();
    }
}

int main()
{
    load_games_from_file("games.txt");
    load_installed_games_from_file("installed_games.txt");
    run_program();
    return 0;
}
