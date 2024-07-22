#include <stdio.h>
#include <windows.h>
#include <commdlg.h>

int main() {
    
    char szFileName[MAX_PATH] = {0};

    OPENFILENAMEA ofn = {0};
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Planilha do Microsoft Excel (*.xlsx)\0*.xlsx\0Todos os Arquivos(*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = NULL;  
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

   
    if (GetOpenFileNameA(&ofn)) {
        
        printf("Arquivo Selecionado: %s\n", szFileName);
    } else {
        printf("Cancelada a Seleção de arquivos.\n");
    }

    return 0;
}
