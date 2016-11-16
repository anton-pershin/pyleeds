#include <iostream>
#include <iomanip>
#include "channelflow/dns.h"
#include "channelflow/flowfield.h"
#include "channelflow/utilfuncs.h"
#include "thequick_light/iniparser_light.h"

#include <fstream>

using namespace std;
using namespace channelflow;
using namespace thequicklight;

int main()
{
    cout << "================================================================\n";
    cout << "This program integrates a plane Couette flow from a random\n";
    cout << "initial condition. Velocity fields are saved at intervals dT=1.0\n";
    cout << "in a data-couette/ directory, in channelflow's binary data file\n";
    cout << "format." << endl << endl;

    IniParser parser("settings.ini");

    // Define gridsize
    const int Nx = parser.getValue<int>("Definitions", "Nx");
    const int Ny = parser.getValue<int>("Definitions", "Ny");
    const int Nz = parser.getValue<int>("Definitions", "Nz");

    // Define box size
    const int LxPrefactor = parser.getValue<int>("Definitions", "LxPrefactor");
    const Real a = parser.getValue<float>("Definitions", "a");
    const Real b = parser.getValue<float>("Definitions", "b");
    const int LzPrefactor = parser.getValue<int>("Definitions", "LzPrefactor");

    const Real Lx=LxPrefactor*pi;
    const Real Lz=LzPrefactor*pi;

    cout << "Nx = " << Nx << ", Ny = " << Ny << ", Nz = " << Nz << endl << endl;
    cout << "Lx = " << LxPrefactor << "*pi, Ly = " << b - a << ", Lz = " << LzPrefactor << "*pi" << endl << endl;
        
    // Define flow parameters
    //const Real Reynolds = 400.0;
    const Real Reynolds = parser.getValue<float>("Definitions", "Re");
    const Real nu = 1.0/Reynolds;
    const Real dPdx  = 0.0;
    
    // Define integration parameters
    //const int   n = 400;    // take n steps between printouts
    const int   n = ::floor(Reynolds);    // take n steps between printouts
    const Real dt = 1.0/n; // integration timestep
    //const Real T  = 100.0; // integrate from t=0 to t=T
    
    // Define DNS parameters
    DNSFlags flags;
    flags.baseflow     = PlaneCouette;
    flags.timestepping = SBDF3;
    flags.initstepping = SMRK2;
    flags.nonlinearity = Rotational;
    flags.dealiasing   = DealiasXZ;
    //flags.nonlinearity = SkewSymmetric;
    //flags.dealiasing   = NoDealiasing;
    flags.taucorrection = true;
    flags.constraint  = PressureGradient; // enforce constant pressure gradient
    flags.dPdx  = dPdx;

    const Real T0 = 0;
    const Real T1 = parser.getValue<int>("Definitions", "T");
    //flags.t0    = T0;
    
    // Define size and smoothness of initial disturbance
    Real spectralDecay = 0.5;
    Real magnitude  = 0.3;
    int kxmax = 3;
    int kzmax = 3;
    
    bool startFromState = false;
    IniParser::ErrorCode err;
    string uFile = parser.getValue<string>("Initial conditions", "U_file", &err);
    if (err == IniParser::ErrorCode::Success)
    {
        startFromState = true;
    }

    // Construct data fields: 3d velocity and 1d pressure
    cout << "building velocity and pressure fields..." << flush;
    FlowField u;
    FlowField q;
    if (startFromState)
    {
        u = FlowField("data-couette/u90");
        q = FlowField(u.Nx(), u.Ny(), u.Nz(), 1, u.Lx(), u.Lz(), u.a(), u.b());
    }
    else
    {
        u = FlowField(Nx,Ny,Nz,3,Lx,Lz,a,b);
        q = FlowField(Nx,Ny,Nz,1,Lx,Lz,a,b);
    }

    cout << "done" << endl;
    
    // Perturb velocity field
    if (!startFromState)
    {
        u.addPerturbations(kxmax,kzmax,1.0,spectralDecay);
        u *= magnitude/L2Norm(u);
    }

    // Construct Navier-Stoke integrator, set integration method
    cout << "building DNS..." << flush;
    DNS dns(u, nu, dt, flags);
    cout << "done" << endl;
    
    mkdir("data-couette");
    fstream u_file("u_norms", ios_base::out);
    fstream v_file("v_norms", ios_base::out);
    fstream w_file("w_norms", ios_base::out);
    fstream ke_file("ke", ios_base::out);
    for (Real t = T0; t <= T1; t += n*dt)
    {
        cout << "         t == " << t << endl;
        cout << "       CFL == " << dns.CFL() << endl;
        cout << " L2Norm(u) == " << L2Norm(u[0]) << endl;
        cout << " L2Norm(v) == " << L2Norm(u[1]) << endl;
        cout << " L2Norm(w) == " << L2Norm(u[2]) << endl;
        cout << "divNorm(u) == " << divNorm(u) << endl;
        cout << "      dPdx == " << dns.dPdx() << endl;
        cout << "     Ubulk == " << dns.Ubulk() << endl;
        
        u_file << L2Norm(u[0]) << ",";
        v_file << L2Norm(u[1]) << ",";
        w_file << L2Norm(u[2]) << ",";
        ke_file << L2Norm(u[0])*L2Norm(u[0]) + L2Norm(u[1])*L2Norm(u[1]) + L2Norm(u[2])*L2Norm(u[2]) << ",";
        // Write velocity and modified pressure fields to disk
        if (!startFromState)
        {
            u.makePhysical();
            u.save("data-couette/u"+i2s(int(t)));
            q.save("data-couette/q"+i2s(int(t)));
            u.makeSpectral();
        }
        
        // Take n steps of length dt
        dns.advance(u, q, n);
        cout << endl;
    }
}
