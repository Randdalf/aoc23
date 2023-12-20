#include "aoc.h"
#include "parse.h"

const char* DefaultInputPath = "d20.txt";

enum
{
    MODULE_BROADCASTER = 'b',
    MODULE_FLIP_FLOP = '%',
    MODULE_CONJUNCTION = '&'
};

static bool IsModule(char C)
{
    return C == MODULE_BROADCASTER
        || C == MODULE_FLIP_FLOP
        || C == MODULE_CONJUNCTION;
}

static bool IsLower(char C)
{
    return C >= 'a' && C <= 'z';
}

enum
{
    STATE_OFF,
    STATE_ON
};

typedef struct
{
    uint16_t State;
    uint16_t OutputIds[7];
    uint8_t OutputPins[7];
    uint8_t Type;
    uint8_t NumInputs;
    uint8_t NumOutputs;
} module;

typedef struct
{
    uint16_t Id;
    uint8_t Value;
    uint8_t Pin;
} pulse;

static const char* ParseId(const char* Input, uint16_t* OutId)
{
    uint16_t Id = 0;
    while(IsLower(*Input))
    {
        Id = Id * 26 + *Input++ - 'a';
    }
    *OutId = Id + 1;
    return Input;
}

typedef struct
{
    size_t Count;
    size_t Capacity;
    pulse* Buffer;
    int Head;
    int Tail;
} pulse_queue;

static void InitPulseQueue(pulse_queue* Queue)
{
    Queue->Capacity = 0;
    Queue->Buffer = NULL;
    Queue->Head = 0;
    Queue->Tail = 0;
}

static void FreePulseQueue(pulse_queue* Queue)
{
    free(Queue->Buffer);
}

static void PulseQueuePush(pulse_queue* Queue, uint16_t Id, uint8_t Value, uint8_t Pin)
{
    if(Queue->Count == Queue->Capacity)
    {
        size_t NewCapacity = Queue->Capacity ? Queue->Capacity * 2 : 64;
        pulse* NewBuffer = (pulse*)malloc(sizeof(pulse) * NewCapacity);
        if(Queue->Head < Queue->Tail)
        {
            memcpy(NewBuffer, Queue->Buffer + Queue->Head, sizeof(pulse) * (Queue->Tail - Queue->Head));
            Queue->Tail -= Queue->Head;
        }
        else
        {
            size_t ToCapacity = Queue->Capacity - Queue->Head;
            memcpy(NewBuffer, Queue->Buffer + Queue->Head, sizeof(pulse) * ToCapacity);
            memcpy(NewBuffer + ToCapacity, Queue->Buffer, sizeof(pulse) * Queue->Tail);
            Queue->Tail += ToCapacity;
        }
        Queue->Head = 0;
        free(Queue->Buffer);
        Queue->Capacity = NewCapacity;
        Queue->Buffer = NewBuffer;
    }
    Queue->Buffer[Queue->Tail] = (pulse){.Id = Id, .Value = Value, .Pin = Pin};
    Queue->Tail = (Queue->Tail + 1) & (Queue->Capacity - 1);
    Queue->Count++;
}

static pulse PulseQueuePull(pulse_queue* Queue)
{
    pulse Pulse = Queue->Buffer[Queue->Head];
    Queue->Head = (Queue->Head + 1) & (Queue->Capacity - 1);
    Queue->Count--;
    return Pulse;
}

typedef void (*on_pulse_fn)(void* User, int64_t Push, pulse Pulse);
typedef bool (*should_push_fn)(void* User, int64_t Push);

static void Solve(const char* Input, should_push_fn ShouldPushFn, on_pulse_fn OnPulseFn, void* User)
{
    module* Modules = (module*)calloc(26 * 26 + 1, sizeof(module));
    while(IsModule(*Input))
    {
        uint8_t Type = *Input++;
        uint16_t Id;
        if(Type == MODULE_BROADCASTER)
        {
            Id = 0;
            Input += 14;
        }
        else
        {
            Input = ParseId(Input, &Id) + 4;
        }
        module* Module = &Modules[Id];
        Module->Type = Type;
        while(IsLower(*Input))
        {
            uint16_t OutputId;
            Input = ParseId(Input, &OutputId);
            if(*Input == ',') Input += 2;
            module* OutputModule = &Modules[OutputId];
            Module->OutputIds[Module->NumOutputs] = OutputId;
            Module->OutputPins[Module->NumOutputs++] = OutputModule->NumInputs++;
        }
        Input = SkipPastLine(Input);
    }
    pulse_queue Pending;
    InitPulseQueue(&Pending);
    for(int64_t Push = 0; ShouldPushFn(User, Push); Push++)
    {
        PulseQueuePush(&Pending, 0, 0, 0);
        while(Pending.Count > 0)
        {
            pulse Pulse = PulseQueuePull(&Pending);
            module* Module = &Modules[Pulse.Id];
            OnPulseFn(User, Push, Pulse);
            switch(Module->Type)
            {
            case MODULE_BROADCASTER:
                for(int Index = 0; Index < Module->NumOutputs; Index++)
                {
                    PulseQueuePush(&Pending, Module->OutputIds[Index], Pulse.Value, Module->OutputPins[Index]);
                }
                break;
            case MODULE_FLIP_FLOP:
                if(!Pulse.Value)
                {
                    Module->State = 1 - Module->State;
                    for(int Index = 0; Index < Module->NumOutputs; Index++)
                    {
                        PulseQueuePush(&Pending, Module->OutputIds[Index], (uint8_t)Module->State, Module->OutputPins[Index]);
                    }
                }
                break;
            case MODULE_CONJUNCTION:
                Module->State = (Module->State & ~(1 << Pulse.Pin)) | (Pulse.Value << Pulse.Pin);
                uint8_t All = __popcnt16(Module->State) != Module->NumInputs;
                for(int Index = 0; Index < Module->NumOutputs; Index++)
                {
                    PulseQueuePush(&Pending, Module->OutputIds[Index], All, Module->OutputPins[Index]);
                }
                break;
            }
        }
    }
    FreePulseQueue(&Pending);
    free(Modules);
}

static bool LessThan1000(void* User, int64_t Push)
{
    AOC_UNUSED(User);
    return Push < 1000;
}

static void CountPulses(void* User, int64_t Push, pulse Pulse)
{
    AOC_UNUSED(Push);
    ((int64_t*)User)[Pulse.Value]++;
}

AOC_SOLVER(Part1)
{
    int64_t Counters[2] = {0, 0};
    Solve(Input, LessThan1000, CountPulses, Counters);
    return Counters[0] * Counters[1];
}

typedef struct
{
    int64_t Pushes[4];
    uint16_t Jz;
    uint16_t NumFound;
} params;

static bool UntilAllJzPushesFound(void* User, int64_t Push)
{
    AOC_UNUSED(Push);
    return ((params*)User)->NumFound < 4;
}

static void FindJzPushes(void* User, int64_t Push, pulse Pulse)
{
    params* Params = (params*)User;
    if(!Pulse.Value || Pulse.Id != Params->Jz) return;
    Params->Pushes[Pulse.Pin] = Push + 1;
    Params->NumFound++;
}

AOC_SOLVER(Part2)
{
    params Params;
    memset(&Params, 0, sizeof(params));
    ParseId("jz", &Params.Jz); // Penultimate conjunction before "rx".
    Solve(Input, UntilAllJzPushesFound, FindJzPushes, &Params);
    return Params.Pushes[0] * Params.Pushes[1] * Params.Pushes[2] * Params.Pushes[3];
}
