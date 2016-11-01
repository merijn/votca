/*
 *            Copyright 2009-2016 The VOTCA Development Team
 *                       (http://www.votca.org)
 *
 *      Licensed under the Apache License, Version 2.0 (the "License")
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef PARALLELPAIRCALC_H
#define PARALLELPAIRCALC_H


#include <votca/xtp/qmcalculator.h>
#include <votca/tools/thread.h>
#include <votca/ctp/qmthread.h>
#include <votca/tools/mutex.h>


namespace votca { namespace ctp {

class ParallelPairCalculator : public XQMCalculator
{

public:

    class PairOperator;
    
    ParallelPairCalculator() : _nextPair(NULL) {};
   ~ParallelPairCalculator() {};

    string       Identify() { return "Parallel pair calculator"; }

    bool         EvaluateFrame(Topology *top);
    virtual void InitSlotData(Topology *top) { ; }
    virtual void PostProcess(Topology *top) { ; }
    virtual void EvalPair(Topology *top, QMPair *qmpair, PairOperator* opThread) { ; }

    QMPair     *RequestNextPair(int opId, Topology *top);
    void         LockCout() { _coutMutex.Lock(); }
    void         UnlockCout() { _coutMutex.Unlock(); }


    // ++++++++++++++++++++++++++++++++++++++ //
    // Pair workers (i.e. individual threads) //
    // ++++++++++++++++++++++++++++++++++++++ //

    class PairOperator : public QMThread
    {
    public:

        PairOperator(int id, Topology *top,
                     ParallelPairCalculator *master)
                   : _top(top), _pair(NULL),
                     _master(master)      { _id = id; };

       ~PairOperator() {};

        void Run(void);
        

    protected:

        Topology                *_top;
        QMPair                 *_pair;
        ParallelPairCalculator  *_master;
    };


protected:

    QMNBList::iterator   _nextPair;
    Mutex                 _nextPairMutex;
    Mutex                 _coutMutex;


};

}}





#endif
