int main(char* data) {
    int i = 0;
    while(data[i]&&(data[i]!='\n')){
        i++;
    }
    return i;
}

