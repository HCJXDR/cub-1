#include <cctest/cctest.h>
#include <cub/dci/Role.h>
#include <cub/base/BaseTypes.h>
#include <cub/base/Keywords.h>

USING_CUB_NS

namespace
{
    DEFINE_ROLE(Energy)
    {
        ABSTRACT(void consume());
        ABSTRACT(bool isExhausted() const);
    };

    struct HumanEnergy : Energy
    {
        HumanEnergy()
        : isHungry(false), consumeTimes(0)
        {
        }

    private:
        OVERRIDE(void consume())
        {
            consumeTimes++;

            if(consumeTimes >= MAX_CONSUME_TIMES)
            {
                isHungry = true;
            }
        }

        OVERRIDE(bool isExhausted() const)
        {
            return isHungry;
        }

    private:
        enum
        {
            MAX_CONSUME_TIMES = 10
        };

        bool isHungry;
        U8 consumeTimes;
    };

    DEFINE_ROLE(Worker)
    {
        Worker() : produceNum(0)
        {
        }

        void produce()
        {
            if(ROLE(Energy).isExhausted()) return;

            produceNum++;

            ROLE(Energy).consume();
        }

        U32 getProduceNum() const
        {
            return produceNum;
        }

    private:
        U32 produceNum;

    private:
        USE_ROLE(Energy);
    };

    DEFINE_ROLE(Human)
    {
        HAS_ROLE(Worker);
    };

    struct HumanObject : Human
                       , private Worker
                       , private HumanEnergy
    {
    private:
        IMPL_ROLE(Worker);
        IMPL_ROLE(Energy);
    };

    struct HumanFactory
    {
        static Human* create()
        {
            return new HumanObject;
        }
    };
}

FIXTURE(HasRoleTest)
{
	TEST("should visit all the role correctly through has role interface")
	{
		Human* human = HumanFactory::create();

		human->ROLE(Worker).produce();

		ASSERT_EQ(1, human->ROLE(Worker).getProduceNum());

		delete human;
	}
};
