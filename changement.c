int print_UART_n(char *toPrint, int n) {
    int i = 0;
    while (print_UART(toPrint[i])==1 && i < n) {
        i++;
    }
	flush();
	if(i<n-1)
		return 0
	else
    return 1;
}
