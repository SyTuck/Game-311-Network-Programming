using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Assets._Complete_Game.Scripts.Responses
{
    [Serializable]
    public class FoodEatenResponseData
    {
        public bool updated;
        public int appleValue;
        public int popValue;
        public string healthMessage;
    }
}
