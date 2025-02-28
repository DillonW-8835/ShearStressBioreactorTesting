
static constexpr double CHANNEL_HEIGHT = 0.0005; // m
static constexpr double CHANNEL_WIDTH = 0.04; // m
static constexpr double MU = 0.0006922; // Pa * s
static constexpr double RHO = 993; // kg / m^3

double flowRateCalc(double sesnorValue) {
    double fl = (((CHANNEL_WIDTH * CHANNEL_HEIGHT * CHANNEL_HEIGHT * sesnorValue) / (6 * MU)) * 6e7)/32;
    return fl;
}

//Calculates the shear stress in Pa that will result from the provided flow rate in ml/min.
double shearStressCalc(double flowRate) {
    double sh = (flowRate / 6e7) * (6 * MU) / (CHANNEL_WIDTH * CHANNEL_HEIGHT * CHANNEL_HEIGHT);
    return sh;
}


//Calculates the Reynolds number for the given flow rate.
double reynoldsCalc(double flowRate) {
    double r = (RHO * (flowRate / (CHANNEL_HEIGHT * CHANNEL_WIDTH)) * CHANNEL_HEIGHT) / MU;
    return r;
}