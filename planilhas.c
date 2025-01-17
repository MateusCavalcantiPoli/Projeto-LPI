#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Estrutura para armazenar o TextView
typedef struct {
    GtkTextBuffer *buffer;
} AppData;

// Função para verificar se a string contém apenas dígitos
int somente_digitos(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

// Função para validar o CEP
int validar_cep(const char *cep) {
    int len = strlen(cep);
    if (len == 8 && somente_digitos(cep)) {
        return 1;
    }
    if (len == 9) {
        if (cep[5] == '-' || cep[5] == ' ') {
            char parte1[6], parte2[4];
            strncpy(parte1, cep, 5);
            parte1[5] = '\0';
            strncpy(parte2, cep + 6, 3);
            parte2[3] = '\0';
            return somente_digitos(parte1) && somente_digitos(parte2);
        }
    }
    return 0;
}

// Função para abrir e processar o arquivo CSV
void abrirCsv(const char *nomeArquivo, AppData *app_data) {
    gtk_text_buffer_set_text(app_data->buffer, "", -1);

    FILE *file = fopen(nomeArquivo, "r");
    if (!file) {
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(app_data->buffer, &end);
        gtk_text_buffer_insert(app_data->buffer, &end, "Não foi possível abrir o arquivo CSV: ", -1);
        gtk_text_buffer_insert(app_data->buffer, &end, nomeArquivo, -1);
        gtk_text_buffer_insert(app_data->buffer, &end, "\n", -1);
        return;
    }

    GtkTextIter end;
    gtk_text_buffer_get_end_iter(app_data->buffer, &end);
    gtk_text_buffer_insert(app_data->buffer, &end, "Arquivo selecionado: ", -1);
    gtk_text_buffer_insert(app_data->buffer, &end, nomeArquivo, -1);
    gtk_text_buffer_insert(app_data->buffer, &end, "\n", -1);

    char line[1024];
    int ln = 1;
    int cep_columns[100];
    int cep_column_count = 0;

    if (fgets(line, sizeof(line), file)) {
        char *token;
        int col = 1;
        token = strtok(line, ";");
        while (token) {
            if (strstr(token, "CEP") != NULL) {
                cep_columns[cep_column_count++] = col;
            }
            token = strtok(NULL, ";");
            col++;
        }
        ln++;
    }

    while (fgets(line, sizeof(line), file)) {
        char *token;
        int col = 1;
        token = strtok(line, ";");
        while (token) {
            // Verificar se o token é nulo ou vazio
            if (token == NULL) {
                char mensagem[128];
                snprintf(mensagem, sizeof(mensagem), "Valor nulo encontrado em L%dC%d\n", ln, col);
                gtk_text_buffer_get_end_iter(app_data->buffer, &end);
                gtk_text_buffer_insert(app_data->buffer, &end, mensagem, -1);
            } else if (strcmp(token, "") == 0 || strcmp(token, "\n") == 0 || strcmp(token, "\r\n") == 0) {
                char mensagem[128];
                snprintf(mensagem, sizeof(mensagem), "Célula vazia encontrada em L%dC%d\n", ln, col);
                gtk_text_buffer_get_end_iter(app_data->buffer, &end);
                gtk_text_buffer_insert(app_data->buffer, &end, mensagem, -1);
            }

            // Verificar e validar colunas CEP
            for (int i = 0; i < cep_column_count; i++) {
                if (col == cep_columns[i]) {
                    if (!validar_cep(token)) {
                        char mensagem[128];
                        snprintf(mensagem, sizeof(mensagem), "CEP inválido encontrado em L%dC%d\n", ln, col);
                        gtk_text_buffer_get_end_iter(app_data->buffer, &end);
                        gtk_text_buffer_insert(app_data->buffer, &end, mensagem, -1);
                    }
                }
            }

            token = strtok(NULL, ";");
            col++;
        }
        ln++;
    }

    fclose(file);
}

void on_button_clicked(GtkWidget *widget, gpointer data) {
    AppData *app_data = (AppData *)data;
    const char *nomeArquivo = gtk_button_get_label(GTK_BUTTON(widget));
    abrirCsv(nomeArquivo, app_data);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Escolha uma Planilha");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    // Resultados pelo TextView
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    AppData app_data = { buffer };

    // Definir os arquivos CSV
    const char *file1 = "1.csv";
    const char *file2 = "2.csv";
    const char *file3 = "3.csv";

    // Criar botões para cada arquivo CSV
    GtkWidget *button1 = gtk_button_new_with_label(file1);
    g_signal_connect(button1, "clicked", G_CALLBACK(on_button_clicked), &app_data);
    gtk_box_pack_start(GTK_BOX(hbox), button1, TRUE, TRUE, 0);

    GtkWidget *button2 = gtk_button_new_with_label(file2);
    g_signal_connect(button2, "clicked", G_CALLBACK(on_button_clicked), &app_data);
    gtk_box_pack_start(GTK_BOX(hbox), button2, TRUE, TRUE, 0);

    GtkWidget *button3 = gtk_button_new_with_label(file3);
    g_signal_connect(button3, "clicked", G_CALLBACK(on_button_clicked), &app_data);
    gtk_box_pack_start(GTK_BOX(hbox), button3, TRUE, TRUE, 0);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
