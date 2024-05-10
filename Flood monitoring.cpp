#include<iostream>
#include<bitset>
#include<cmath>
#include<fstream>

using std::cin;
using std::cout;
using std::endl;
using std::bitset;
using std::ofstream;

double calculateNTU(double);

int main() {

	const double startVoltage = 1.50293;
	const double endVoltage = 3.56250;
	const double Vref = 3.8;
	const int	 adcResolution = 10;
	int	         adcSteps = pow(2.0, adcResolution);
	int			 adcValue = 0;
	int			 numberOfValues = 0;;
	double       vStep = Vref / adcSteps;
	ofstream		 lookUpTable;

	lookUpTable.open("NTULookup.txt");
	if (!lookUpTable) {
		cout << "Error creating file!" << endl;
	}
	else {
		lookUpTable << "LkUpLo" << endl;
		lookUpTable << "\taddwf PCL,F" << endl;
		for (double i = startVoltage; i <= endVoltage + vStep; i += vStep) {
			adcValue = i / vStep;
			cout << i << "\t" << calculateNTU(i) << "\t" << adcValue << "\t" << bitset<16>(adcValue) << endl;
			lookUpTable << "\tretlw B'" << bitset<8>(adcValue) << "'  ;" << calculateNTU(i) << endl;
			numberOfValues++;
		}


		lookUpTable << "\n\nLkUpHi" << endl;
		lookUpTable << "\taddwf PCL,F" << endl;
		for (double i = startVoltage; i <= endVoltage + vStep; i += vStep) {
			adcValue = i / vStep;
			cout << i << "\t" << calculateNTU(i) << "\t" << adcValue << "\t" << bitset<16>(adcValue) << endl;
			lookUpTable << "\tretlw B'" << bitset<8>(adcValue>>8) << "'  ;" << calculateNTU(i) << endl;
		}
		cout << numberOfValues << endl;

	}




	return 0;
}

double calculateNTU(double voltage) {
	const double yInt = 3.565;
	const double grad = -0.0013;
    double NTU  = 0.0;
	NTU = (voltage - yInt) / grad;

	return NTU;

}