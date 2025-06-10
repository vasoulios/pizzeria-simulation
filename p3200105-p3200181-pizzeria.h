#pragma once
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

//variable declaration

const int Ncook = 2;		// cooks
const int Noven = 15;		// ovens
const int Npacker = 2;		// packing employees
const int Ndeliverer = 10;	// delivery employees

const int Torderlow = 60;	// time interval in the [Torderlow,Torderhigh] range to connect next costumer
const int Torderhigh = 180;

const int Norderlow = 1;	// costumer orders a random integer number of pizzas in the [Norderlow,Norderhigh] range
const int Norderhigh = 5;

const double Pplain = 0.6; // plain pizza probability
const double Pspecial = 0.4; // special pizza probability

const int Tpaymentlow = 60;	// time interval in the [Tpaymentlow,Tpaymenthigh] range to charge the costumer
const int Tpaymenthigh = 180;

const double Pfail = 0.10;	// order failure probability

const int Cplain = 10;		// plain pizza cost
const int Cspecial = 12;	// special pizza cost

const int Tprep = 60;		// time needed for cook to prepare pizza
const int Tbake = 600;		// pizzas are baked for Tbake time
const int Tpack = 60;		// time needed to pack each pizza

const int Tdellow = 300;	// time interval in the [Tdellow,Tdelhigh] range to deliver the order
const int Tdelhigh = 900;
