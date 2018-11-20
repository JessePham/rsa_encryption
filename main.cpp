#include <iostream>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <Windows.h>

using namespace std;

void encrypt(ofstream&, string, int[], long, long);
void decrypt(ofstream&, int[], int[], int, long);

void getPrimes(int&, int&, long&, long&);
void chooseE(int, int, long, int&, long[], long[]);
int calcD(int, int);
bool checkPrime(int);

void displayED(long[], long[], int, int&);
void errorMessage(int);

int main() {
	const int MAX_CAP = 500;
	int p = 0, q = 0; // prime numbers p and q
	long n = 0, t = 0; // n is modulus for pub key and private key (p*q), t is totient o(n) = (p - 1)(q - 1)
	int userChoice;	// choice for e and d pair
	long e[100]; // Array containing possible values for e
	long d[100]; // Array containing possible values for d
	int numOptions = 0;	// Holds the number of options available for userChoice
	int encryptedM[MAX_CAP];
	int decryptedM[MAX_CAP];
	ifstream readTextFile;
	ofstream encryptedFile, decryptedFile;

	readTextFile.open("message.txt");
	encryptedFile.open("encrypted.txt");
	decryptedFile.open("decrypted.txt");

	if (!readTextFile.is_open()) {
		errorMessage(3);
		cout << "\nError opening message.txt\n\n";
	}
	if (!encryptedFile.is_open()) {
		errorMessage(3);
		cout << "\nError opening encrypted.txt\n";
	}
	if (!decryptedFile.is_open()) {
		errorMessage(3);
		cout << "\nError opening decrypted.txt\n";
	}

	if (readTextFile.is_open() && encryptedFile.is_open() && decryptedFile.is_open()) {
		getPrimes(p, q, n, t);

		chooseE(p, q, t, numOptions, e, d);

		displayED(e, d, numOptions, userChoice);

		string file_message{ istreambuf_iterator<char>(readTextFile), istreambuf_iterator<char>() };

		encrypt(encryptedFile, file_message, encryptedM, e[userChoice], n);
		decrypt(decryptedFile, encryptedM, decryptedM, d[userChoice], n);

		readTextFile.close();
		encryptedFile.close();
		decryptedFile.close();
	}

	system("PAUSE");
	return 0;
}

// Encrypting the Original Message
void encrypt(ofstream& enFile, string originalM, int eM[], long key, long n) {
	HANDLE color = GetStdHandle(STD_OUTPUT_HANDLE);

	int len, m, counter = 0;
	char charTemp;

	len = originalM.length();

	for (int i = 0; i < len; i++) {

		long long k = 1;	// Had to use long long because demonstration of bigger key values

		m = originalM[i];

		for (int j = 0; j < key; j++) {	// algorithm: M^e mod n
			k = k * m;
			k = k % n;
		}

		eM[i] = k;

		counter = i + 1;
	}

	eM[counter] = -1;	// Marking end of the string

	for (int v = 0; eM[v] != -1; v++) {
		charTemp = eM[v];
		enFile << charTemp;
	}

	SetConsoleTextAttribute(color, 252);
	cout << "\nThe Encrypted Message has been sent!\n";
	SetConsoleTextAttribute(color, 7);
}

// Decrypting the Original Message
void decrypt(ofstream& decFile, int encM[], int decM[], int key, long n) {
	HANDLE color = GetStdHandle(STD_OUTPUT_HANDLE);

	int b, count = 0;
	char charTemp;

	for (int i = 0; encM[i] != -1; i++) {

		long long k = 1;

		b = encM[i];

		for (int j = 0; j < key; j++) {
			k = k * b;
			k = k % n;
		}

		decM[i] = k;

		count = i + 1;
	}

	decM[count] = -1;

	for (int v = 0; decM[v] != -1; v++) {
		charTemp = decM[v];
		decFile << charTemp;
	}

	SetConsoleTextAttribute(color, 252);
	cout << "\nThe Decrypted Message has been sent!\n\n";
	SetConsoleTextAttribute(color, 7);
}

// Obtain Prime Numbers p and q ... also Calculates n and t
void getPrimes(int &getP, int &getQ, long &n, long &totient) {
	HANDLE color = GetStdHandle(STD_OUTPUT_HANDLE);

	bool primeFlag;

	SetConsoleTextAttribute(color, 10);
	cout << "Enter in First Prime Number:\np = ";
	cin >> getP;

	primeFlag = checkPrime(getP);

	if (primeFlag == false) {
		do {
			errorMessage(1);

			SetConsoleTextAttribute(color, 10);
			cout << "Enter in First Prime Number:\np = ";
			cin >> getP;

			primeFlag = checkPrime(getP);
		} while (primeFlag == false);
	}

	system("CLS");
	SetConsoleTextAttribute(color, 10);
	cout << "Enter in First Prime Number:\np = " << getP << endl;
	SetConsoleTextAttribute(color, 14);
	cout << "Enter in Second Prime Number:\nq = ";
	cin >> getQ;

	primeFlag = checkPrime(getQ);

	if (primeFlag == false || getP == getQ) {
		do {
			if (primeFlag == false)
				errorMessage(1);
			else if (getP == getQ)
				errorMessage(2);

			SetConsoleTextAttribute(color, 10);
			cout << "Enter in First Prime Number:\np = " << getP << endl;
			SetConsoleTextAttribute(color, 14);
			cout << "Enter in Second Prime Number:\nq = ";
			cin >> getQ;

			primeFlag = checkPrime(getQ);

		} while (primeFlag == false);
	}

	system("CLS");
	SetConsoleTextAttribute(color, 10);
	cout << "Enter in First Prime Number:\np = " << getP << endl;
	SetConsoleTextAttribute(color, 14);
	cout << "Enter in Second Prime Number:\nq = " << getQ << endl;

	n = getP * getQ;
	totient = (getP - 1)*(getQ - 1);
}

// Obtains values for e and d then stores them in an array
void chooseE(int primeP, int primeQ, long totient, int &nOpt, long arrayE[], long arrayD[]) {
	int k = 0;
	bool flag;
	int dFlag;

	for (int i = 2; i < totient; i++) {

		if (totient % i == 0)	// i is not relatively prime to totient
			continue;

		flag = checkPrime(i);

		// i is relatively prime to totient
		if (flag == true && i != primeP && i != primeQ) {

			nOpt++;
			arrayE[k] = i;
			dFlag = calcD(arrayE[k], totient);

			if (dFlag > 0)
				arrayD[k] = dFlag;

			if (k  == 15)
				break;

			k++;
		}
	}
}

// Calculates d value d = (1+(k*z))/e -> whole number
int calcD(int e, int totient) {
	int k = 1;

	while (true) {
		if ((1 + (k * totient)) % e == 0)
			return (1 + (k * totient) / e);
		else
			k++;
	}
}

// Checks if number a is a prime number
bool checkPrime(int a) {
	for (int i = 2; i <= sqrt(a); i++) {
		if (a % i == 0) {
			return false;
		}
	}
	return true;
}

// Displays the first 15 values of possible e and d choices
void displayED(long arrE[], long arrD[], int nOpt, int &user) {
	HANDLE color = GetStdHandle(STD_OUTPUT_HANDLE);
	int vbar = 186,
		hbar = 205,
		topr = 187,
		botr = 188,
		topl = 201,
		botl = 200;

	int counter = 0;

	SetConsoleTextAttribute(color, 15);
	cout << (char)topl;
	for (int m = 0; m < 36; m++)
		cout << (char)hbar;
	cout << (char)topr << endl;

	cout << (char)vbar;
	cout << "Possible Values of ";

	SetConsoleTextAttribute(color, 11);
	cout << "'e' ";

	SetConsoleTextAttribute(color, 15);
	cout << "and ";

	SetConsoleTextAttribute(color, 13);
	cout << "'d' ";
	SetConsoleTextAttribute(color, 15);
	cout << "are: ";
	cout << (char)vbar << endl;

	cout << (char)204;
	for (int p = 0; p < 36; p++)
		cout << (char)hbar;
	cout << (char)185 << endl;

	for (int i = 0; i < nOpt - 1; i++) {	// nOpt: How many options do you want to display
		SetConsoleTextAttribute(color, 15);
		cout << (char)vbar << i + 1 << ":";
		SetConsoleTextAttribute(color, 11);
		cout << "\t" << arrE[i] << "\t\t";
		SetConsoleTextAttribute(color, 13);
		cout << arrD[i] << "\t     ";
		SetConsoleTextAttribute(color, 15);
		cout << (char)vbar << endl;

		counter = i + 1;
	}

	cout << (char)botl;
	for (int n = 0; n < 36; n++)
		cout << (char)hbar;
	cout << (char)botr << endl;

	SetConsoleTextAttribute(color, 15);
	cout << "Enter your choice: ";
	cin >> user;

	if (user < 1 || user > counter) {
		do {
			errorMessage(2);

			SetConsoleTextAttribute(color, 15);
			cout << "Enter your choice: ";
			cin >> user;
		} while (user < 1 || user > counter);
	}

	SetConsoleTextAttribute(color, 7);
}

// Error Messages
void errorMessage(int choice) {
	HANDLE color = GetStdHandle(STD_OUTPUT_HANDLE);

	//system("CLS");
	SetConsoleTextAttribute(color, 207);
	cout << "\nERROR!!!";
	SetConsoleTextAttribute(color, 12);
	switch (choice) {
	case 1: cout << "\nThat is not a prime number!\nTry again.\n\n";
		break;
	case 2: cout << "\nThat is not a valid choice!\nTry again.\n\n";
		break;
	default:
		break;
	}
}