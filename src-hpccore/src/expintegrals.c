###########################################################################
# ALGLIB 4.01.0 (source code generated 2023-12-27)
# Copyright (c) Sergey Bochkanov (ALGLIB project).
# 
# >>> SOURCE LICENSE >>>
# This software is a non-commercial edition of  ALGLIB  package,  which  is
# licensed under ALGLIB Personal and Academic Use License Agreement (PAULA).
# 
# See paula-v1.0.pdf file in the archive  root  for  full  text  of  license
# agreement.
# >>> END OF LICENSE >>>

##########################################################################


#include <stdafx.h>
#include "expintegrals.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
Exponential integral Ei(x)

              x
               -     t
              | |   e
   Ei(x) =   -|-   ---  dt .
            | |     t
             -
            -inf

Not defined for x <= 0.
See also expn.c.



ACCURACY:

                     Relative error:
arithmetic   domain     # trials      peak         rms
   IEEE       0,100       50000      8.6e-16     1.3e-16

Cephes Math Library Release 2.8:  May, 1999
Copyright 1999 by Stephen L. Moshier
*************************************************************************/
double exponentialintegralei(double x, ae_state *_state)
{
    double eul;
    double f;
    double f1;
    double f2;
    double w;
    double result;


    eul = 0.5772156649015328606065;
    if( ae_fp_less_eq(x,(double)(0)) )
    {
        result = (double)(0);
        return result;
    }
    if( ae_fp_less(x,(double)(2)) )
    {
        f1 = -5.350447357812542947283;
        f1 = f1*x+218.5049168816613393830;
        f1 = f1*x-4176.572384826693777058;
        f1 = f1*x+55411.76756393557601232;
        f1 = f1*x-331338.1331178144034309;
        f1 = f1*x+1592627.163384945414220;
        f2 = 1.000000000000000000000;
        f2 = f2*x-52.50547959112862969197;
        f2 = f2*x+1259.616186786790571525;
        f2 = f2*x-17565.49581973534652631;
        f2 = f2*x+149306.2117002725991967;
        f2 = f2*x-729494.9239640527645655;
        f2 = f2*x+1592627.163384945429726;
        f = f1/f2;
        result = eul+ae_log(x, _state)+x*f;
        return result;
    }
    if( ae_fp_less(x,(double)(4)) )
    {
        w = (double)1/x;
        f1 = 1.981808503259689673238E-2;
        f1 = f1*w-1.271645625984917501326;
        f1 = f1*w-2.088160335681228318920;
        f1 = f1*w+2.755544509187936721172;
        f1 = f1*w-4.409507048701600257171E-1;
        f1 = f1*w+4.665623805935891391017E-2;
        f1 = f1*w-1.545042679673485262580E-3;
        f1 = f1*w+7.059980605299617478514E-5;
        f2 = 1.000000000000000000000;
        f2 = f2*w+1.476498670914921440652;
        f2 = f2*w+5.629177174822436244827E-1;
        f2 = f2*w+1.699017897879307263248E-1;
        f2 = f2*w+2.291647179034212017463E-2;
        f2 = f2*w+4.450150439728752875043E-3;
        f2 = f2*w+1.727439612206521482874E-4;
        f2 = f2*w+3.953167195549672482304E-5;
        f = f1/f2;
        result = ae_exp(x, _state)*w*((double)1+w*f);
        return result;
    }
    if( ae_fp_less(x,(double)(8)) )
    {
        w = (double)1/x;
        f1 = -1.373215375871208729803;
        f1 = f1*w-7.084559133740838761406E-1;
        f1 = f1*w+1.580806855547941010501;
        f1 = f1*w-2.601500427425622944234E-1;
        f1 = f1*w+2.994674694113713763365E-2;
        f1 = f1*w-1.038086040188744005513E-3;
        f1 = f1*w+4.371064420753005429514E-5;
        f1 = f1*w+2.141783679522602903795E-6;
        f2 = 1.000000000000000000000;
        f2 = f2*w+8.585231423622028380768E-1;
        f2 = f2*w+4.483285822873995129957E-1;
        f2 = f2*w+7.687932158124475434091E-2;
        f2 = f2*w+2.449868241021887685904E-2;
        f2 = f2*w+8.832165941927796567926E-4;
        f2 = f2*w+4.590952299511353531215E-4;
        f2 = f2*w+(-4.729848351866523044863E-6);
        f2 = f2*w+2.665195537390710170105E-6;
        f = f1/f2;
        result = ae_exp(x, _state)*w*((double)1+w*f);
        return result;
    }
    if( ae_fp_less(x,(double)(16)) )
    {
        w = (double)1/x;
        f1 = -2.106934601691916512584;
        f1 = f1*w+1.732733869664688041885;
        f1 = f1*w-2.423619178935841904839E-1;
        f1 = f1*w+2.322724180937565842585E-2;
        f1 = f1*w+2.372880440493179832059E-4;
        f1 = f1*w-8.343219561192552752335E-5;
        f1 = f1*w+1.363408795605250394881E-5;
        f1 = f1*w-3.655412321999253963714E-7;
        f1 = f1*w+1.464941733975961318456E-8;
        f1 = f1*w+6.176407863710360207074E-10;
        f2 = 1.000000000000000000000;
        f2 = f2*w-2.298062239901678075778E-1;
        f2 = f2*w+1.105077041474037862347E-1;
        f2 = f2*w-1.566542966630792353556E-2;
        f2 = f2*w+2.761106850817352773874E-3;
        f2 = f2*w-2.089148012284048449115E-4;
        f2 = f2*w+1.708528938807675304186E-5;
        f2 = f2*w-4.459311796356686423199E-7;
        f2 = f2*w+1.394634930353847498145E-8;
        f2 = f2*w+6.150865933977338354138E-10;
        f = f1/f2;
        result = ae_exp(x, _state)*w*((double)1+w*f);
        return result;
    }
    if( ae_fp_less(x,(double)(32)) )
    {
        w = (double)1/x;
        f1 = -2.458119367674020323359E-1;
        f1 = f1*w-1.483382253322077687183E-1;
        f1 = f1*w+7.248291795735551591813E-2;
        f1 = f1*w-1.348315687380940523823E-2;
        f1 = f1*w+1.342775069788636972294E-3;
        f1 = f1*w-7.942465637159712264564E-5;
        f1 = f1*w+2.644179518984235952241E-6;
        f1 = f1*w-4.239473659313765177195E-8;
        f2 = 1.000000000000000000000;
        f2 = f2*w-1.044225908443871106315E-1;
        f2 = f2*w-2.676453128101402655055E-1;
        f2 = f2*w+9.695000254621984627876E-2;
        f2 = f2*w-1.601745692712991078208E-2;
        f2 = f2*w+1.496414899205908021882E-3;
        f2 = f2*w-8.462452563778485013756E-5;
        f2 = f2*w+2.728938403476726394024E-6;
        f2 = f2*w-4.239462431819542051337E-8;
        f = f1/f2;
        result = ae_exp(x, _state)*w*((double)1+w*f);
        return result;
    }
    if( ae_fp_less(x,(double)(64)) )
    {
        w = (double)1/x;
        f1 = 1.212561118105456670844E-1;
        f1 = f1*w-5.823133179043894485122E-1;
        f1 = f1*w+2.348887314557016779211E-1;
        f1 = f1*w-3.040034318113248237280E-2;
        f1 = f1*w+1.510082146865190661777E-3;
        f1 = f1*w-2.523137095499571377122E-5;
        f2 = 1.000000000000000000000;
        f2 = f2*w-1.002252150365854016662;
        f2 = f2*w+2.928709694872224144953E-1;
        f2 = f2*w-3.337004338674007801307E-2;
        f2 = f2*w+1.560544881127388842819E-3;
        f2 = f2*w-2.523137093603234562648E-5;
        f = f1/f2;
        result = ae_exp(x, _state)*w*((double)1+w*f);
        return result;
    }
    w = (double)1/x;
    f1 = -7.657847078286127362028E-1;
    f1 = f1*w+6.886192415566705051750E-1;
    f1 = f1*w-2.132598113545206124553E-1;
    f1 = f1*w+3.346107552384193813594E-2;
    f1 = f1*w-3.076541477344756050249E-3;
    f1 = f1*w+1.747119316454907477380E-4;
    f1 = f1*w-6.103711682274170530369E-6;
    f1 = f1*w+1.218032765428652199087E-7;
    f1 = f1*w-1.086076102793290233007E-9;
    f2 = 1.000000000000000000000;
    f2 = f2*w-1.888802868662308731041;
    f2 = f2*w+1.066691687211408896850;
    f2 = f2*w-2.751915982306380647738E-1;
    f2 = f2*w+3.930852688233823569726E-2;
    f2 = f2*w-3.414684558602365085394E-3;
    f2 = f2*w+1.866844370703555398195E-4;
    f2 = f2*w-6.345146083130515357861E-6;
    f2 = f2*w+1.239754287483206878024E-7;
    f2 = f2*w-1.086076102793126632978E-9;
    f = f1/f2;
    result = ae_exp(x, _state)*w*((double)1+w*f);
    return result;
}


/*************************************************************************
Exponential integral En(x)

Evaluates the exponential integral

                inf.
                  -
                 | |   -xt
                 |    e
     E (x)  =    |    ----  dt.
      n          |      n
               | |     t
                -
                 1


Both n and x must be nonnegative.

The routine employs either a power series, a continued
fraction, or an asymptotic formula depending on the
relative values of n and x.

ACCURACY:

                     Relative error:
arithmetic   domain     # trials      peak         rms
   IEEE      0, 30       10000       1.7e-15     3.6e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1985, 2000 by Stephen L. Moshier
*************************************************************************/
double exponentialintegralen(double x, ae_int_t n, ae_state *_state)
{
    double r;
    double t;
    double yk;
    double xk;
    double pk;
    double pkm1;
    double pkm2;
    double qk;
    double qkm1;
    double qkm2;
    double psi;
    double z;
    ae_int_t i;
    ae_int_t k;
    double big;
    double eul;
    double result;


    eul = 0.57721566490153286060;
    big = 1.44115188075855872*ae_pow((double)(10), (double)(17), _state);
    if( ((n<0||ae_fp_less(x,(double)(0)))||ae_fp_greater(x,(double)(170)))||(ae_fp_eq(x,(double)(0))&&n<2) )
    {
        result = (double)(-1);
        return result;
    }
    if( ae_fp_eq(x,(double)(0)) )
    {
        result = (double)1/(double)(n-1);
        return result;
    }
    if( n==0 )
    {
        result = ae_exp(-x, _state)/x;
        return result;
    }
    if( n>5000 )
    {
        xk = x+(double)n;
        yk = (double)1/(xk*xk);
        t = (double)(n);
        result = yk*t*((double)6*x*x-(double)8*t*x+t*t);
        result = yk*(result+t*(t-2.0*x));
        result = yk*(result+t);
        result = (result+(double)1)*ae_exp(-x, _state)/xk;
        return result;
    }
    if( ae_fp_less_eq(x,(double)(1)) )
    {
        psi = -eul-ae_log(x, _state);
        for(i=1; i<=n-1; i++)
        {
            psi = psi+(double)1/(double)i;
        }
        z = -x;
        xk = (double)(0);
        yk = (double)(1);
        pk = (double)(1-n);
        if( n==1 )
        {
            result = 0.0;
        }
        else
        {
            result = 1.0/pk;
        }
        do
        {
            xk = xk+(double)1;
            yk = yk*z/xk;
            pk = pk+(double)1;
            if( ae_fp_neq(pk,(double)(0)) )
            {
                result = result+yk/pk;
            }
            if( ae_fp_neq(result,(double)(0)) )
            {
                t = ae_fabs(yk/result, _state);
            }
            else
            {
                t = (double)(1);
            }
        }
        while(ae_fp_greater_eq(t,ae_machineepsilon));
        t = (double)(1);
        for(i=1; i<=n-1; i++)
        {
            t = t*z/(double)i;
        }
        result = psi*t-result;
        return result;
    }
    else
    {
        k = 1;
        pkm2 = (double)(1);
        qkm2 = x;
        pkm1 = 1.0;
        qkm1 = x+(double)n;
        result = pkm1/qkm1;
        do
        {
            k = k+1;
            if( k%2==1 )
            {
                yk = (double)(1);
                xk = (double)n+(double)(k-1)/(double)2;
            }
            else
            {
                yk = x;
                xk = (double)k/(double)2;
            }
            pk = pkm1*yk+pkm2*xk;
            qk = qkm1*yk+qkm2*xk;
            if( ae_fp_neq(qk,(double)(0)) )
            {
                r = pk/qk;
                t = ae_fabs((result-r)/r, _state);
                result = r;
            }
            else
            {
                t = (double)(1);
            }
            pkm2 = pkm1;
            pkm1 = pk;
            qkm2 = qkm1;
            qkm1 = qk;
            if( ae_fp_greater(ae_fabs(pk, _state),big) )
            {
                pkm2 = pkm2/big;
                pkm1 = pkm1/big;
                qkm2 = qkm2/big;
                qkm1 = qkm1/big;
            }
        }
        while(ae_fp_greater_eq(t,ae_machineepsilon));
        result = result*ae_exp(-x, _state);
    }
    return result;
}


/*$ End $*/
