using System;
using System.Collections.Generic;
using System.Text;

namespace DefinitionSearcher_v1
{
    public class Marker
    {

        public int Reps;
        public string Name;
        public string Id;

        public Marker(string Name1, string Id1, int Reps1 = 0)
        {
            if (Id1 != "")
                Id = Id1;

            if (Name1 != "")
                Name = Name1;

            if (Reps != null)
                Reps = Reps1;
        }
    }
}
    

