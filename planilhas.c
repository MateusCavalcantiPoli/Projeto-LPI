#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Função para abrir e processar o arquivo CSV
void abrirCsv(const char *nomeArquivo) {
    FILE *file = fopen(nomeArquivo, "r");
    if (!file) {
        printf("Não foi possível abrir o arquivo CSV: %s\n", nomeArquivo);
        return;
    }

    printf("Arquivo selecionado: %s\n", nomeArquivo);

    char line[1024];
    int row = 1;
    while (fgets(line, sizeof(line), file)) {
        char *token;
        int col = 1;
        token = strtok(line, ",");
        while (token) {
            if (strcmp(token, "") == 0 || strcmp(token, "\n") == 0 || strcmp(token, "\r\n") == 0) {
                printf("Célula vazia encontrada em R%dC%d\n", row, col);
            }
            token = strtok(NULL, ",");
            col++;
        }
        row++;
    }

    fclose(file);
}

void on_button_clicked(GtkWidget *widget, gpointer data) {
    const char *nomeArquivo = (const char *)data;
    abrirCsv(nomeArquivo);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Escolha uma Planilha");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Definir os arquivos CSV
    const char *file1 = "1.csv"; 
    const char *file2 = "2.csv";
    const char *file3 = "3.csv";

    // Criar botões para cada arquivo CSV
    GtkWidget *button1 = gtk_button_new_with_label("Abrir 1.csv");
    g_signal_connect(button1, "clicked", G_CALLBACK(on_button_clicked), (gpointer)file1);
    gtk_box_pack_start(GTK_BOX(vbox), button1, TRUE, TRUE, 0);

    GtkWidget *button2 = gtk_button_new_with_label("Abrir 2.csv");
    g_signal_connect(button2, "clicked", G_CALLBACK(on_button_clicked), (gpointer)file2);
    gtk_box_pack_start(GTK_BOX(vbox), button2, TRUE, TRUE, 0);

    GtkWidget *button3 = gtk_button_new_with_label("Abrir 3.csv");
    g_signal_connect(button3, "clicked", G_CALLBACK(on_button_clicked), (gpointer)file3);
    gtk_box_pack_start(GTK_BOX(vbox), button3, TRUE, TRUE, 0);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
