#ifndef SERVANT_H
#define SERVANT_H

/*

    This class is designed for performing some qt-dependent actions for ModellingSystem class.
    Please, DO NOT write here other code if you don't know where put it.

*/

#include <QColor>
#include <QDebug>
#include <QDir>
#include <QTextStream>
#include <math.h>
#include <time.h>
#include <algorithm>
#include "constants.h"
#include "robot.h"
#include "hubmodule.h"

class Servant
{
public:
    static Servant& getInstance()
    {
        static Servant instance;
        return instance;
    }

    // Loading robot profile
    Robot * buildRobot(unsigned int number);

    // Loading enviroment profile
    std::vector<EnvObject *> buildEnvironment(std::pair<int, int> mapSize);

    // Get local map scaling for some robot
    // Robot number must be in range 0..ROBOTS-1
    unsigned int getScaling(unsigned int robotNumber)
    {
        if (scaling.find(robotNumber) != scaling.end())
            return scaling.find(robotNumber)->second;
        else
            return 1;
    }

private:
    Servant();
    Servant(Servant const&);
    void operator=(Servant const&);

    std::map<unsigned int, int> scaling;
};

#endif // SERVANT_H

/* Limit line length to 100 characters; highlight 99th column
 * vim: set textwidth=100 colorcolumn=-1:
 */
