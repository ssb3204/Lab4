#include <gtk/gtk.h>
#include <string.h>
#include <math.h>

typedef struct {
    GtkWidget *entry;
} CalcData;

// 계산 함수
gdouble calculate(const gchar *expression) {
    GError *error = NULL;
    gdouble result = g_strtod(expression, &error);

    if (error != NULL) {
        g_error_free(error);
        return NAN; // 에러 발생 시 NaN (Not a Number)을 반환
    }

    return result;
}

// 버튼 클릭 이벤트 핸들러
void on_button_clicked(GtkWidget *button, gpointer data) {
    const gchar *text = gtk_button_get_label(GTK_BUTTON(button));
    const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(((CalcData *)data)->entry));
    gchar *new_text = g_strconcat(current_text, text, NULL);
    gtk_entry_set_text(GTK_ENTRY(((CalcData *)data)->entry), new_text);
    g_free(new_text);
}

// 계산 버튼 클릭 이벤트 핸들러
void on_calculate_clicked(GtkWidget *button, gpointer data) {
    const gchar *expression = gtk_entry_get_text(GTK_ENTRY(((CalcData *)data)->entry));
    
    // 수학적인 계산을 수행하도록 수정
    gdouble result = calculate(expression);

    // 만약 계산이 성공했을 경우
    if (!isnan(result)) {
        gchar *result_text = g_strdup_printf("%.2f", result);
        gtk_entry_set_text(GTK_ENTRY(((CalcData *)data)->entry), result_text);
        g_free(result_text);
    } else {
        gtk_entry_set_text(GTK_ENTRY(((CalcData *)data)->entry), "Error");
    }
}

// 화면 지우기 버튼 클릭 이벤트 핸들러
void on_clear_clicked(GtkWidget *button, gpointer data) {
    gtk_entry_set_text(GTK_ENTRY(((CalcData *)data)->entry), "");
}

int main(int argc, char *argv[]) {
    GtkWidget *window, *grid, *entry, *button;
    CalcData calc_data;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "간단한 계산기");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    grid = gtk_table_new(5, 4, TRUE);
    gtk_container_add(GTK_CONTAINER(window), grid);

    entry = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(grid), entry, 0, 4, 0, 1);
    gtk_entry_set_alignment(GTK_ENTRY(entry), 1.0);

    calc_data.entry = entry;

    gchar *button_labels[] = {"7", "8", "9", "/",
                              "4", "5", "6", "*",
                              "1", "2", "3", "-",
                              "0", ".", "=", "+"};

    for (int i = 0; i < G_N_ELEMENTS(button_labels); i++) {
        button = gtk_button_new_with_label(button_labels[i]);

        if (i < 12) {
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_button_clicked), &calc_data);
        } else if (i == 12) {
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_calculate_clicked), &calc_data);
        } else if (i == 13) {
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_clear_clicked), &calc_data);
        } else {
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_button_clicked), &calc_data);
        }

        gtk_table_attach_defaults(GTK_TABLE(grid), button, i % 4, i % 4 + 1, i / 4 + 1, i / 4 + 2);
    }

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

