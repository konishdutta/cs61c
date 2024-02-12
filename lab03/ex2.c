int source[] = {3, 1, 4, 1, 5, 9, 0}; //s1
int dest[10]; //s2

int fun(int x) {
	return -x * (x + 1);
}

int main() {
    int k; //t0 is the register
    int sum = 0; //s0 represents sum
    for (k = 0; source[k] != 0; k++) {
        dest[k] = fun(source[k]);
        sum += dest[k];
    }
    return sum;
}