using System;
using System.IO;
using System.Linq;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace DefinitionSearcher_v1
{
    public class RecursiveFileProcessor
    {
        Dictionary<string, Marker> markers = new Dictionary<string, Marker>();
        string outputFilePath;

        public void ProcessDirectory(string targetDirectory, int directoryIndex = 0)
        {

            if (directoryIndex == 1)
            {
                outputFilePath = targetDirectory + "\\output.txt";
            }

            string[] fileEntries = Directory.GetFiles(targetDirectory);

            foreach (string fileName in fileEntries)
            {
                ProcessFile(fileName, directoryIndex > 1);
            }
            
            if (directoryIndex == 1)
            {
                foreach (string fileName in fileEntries)
                {
                    if (fileName.EndsWith(".h")) continue;
                    ProcessFile(fileName, true);
                }
            }
            


            string[] subdirectoryEntries = Directory.GetDirectories(targetDirectory);

            foreach (string subdirectory in subdirectoryEntries)
            {
                ProcessDirectory(subdirectory, directoryIndex+1);
            }

            if (directoryIndex == 1)
            {
                WriteNames();
                markers = new Dictionary<string, Marker>();
            }
        }



        private void ProcessFile(string path, bool findReps)
        {
            string[] lines = File.ReadAllLines(path);

            foreach (string line in lines)
            {
                string[] words = Regex.Split(line, @"\s+");
                if (findReps)
                {
                    foreach (var word in words)
                    {
                        var args = word.Split(',');
                        foreach (var arg in args)
                        {
                            if (arg.StartsWith("IDC_") || arg.StartsWith("IDD_"))
                            {
                                if (markers.ContainsKey(arg))
                                {
                                    markers[arg].Reps++;
                                }
                                else
                                {
                                    //Console.ForegroundColor = ConsoleColor.Red;
                                    //Console.WriteLine("Key was not defined: " + arg);
                                    //Console.ResetColor();
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (line.StartsWith("#define"))
                    {

                        if (words.Length >= 3)
                        {
                            if (markers.ContainsKey(words[1])) continue;
                            markers.Add(words[1], new Marker(words[1], words[2]));
                        }
                    }
                }
            }
        }

        public List<Marker> SortMarkersByReps()
        {
            List<Marker> list = new List<Marker>();

            foreach (var kv in markers)
            {
                list.Add(kv.Value);
            }

            return list.OrderBy(x => -x.Reps).ToList(); ;
        }

        public void WriteNames()
        {

            List<Marker> markers = SortMarkersByReps();

            Console.ForegroundColor = ConsoleColor.Magenta;
            Console.WriteLine(new String('-', 40) +"\n" +"File: " + outputFilePath + "\n"+ new String('-', 40));
            Console.ResetColor();

            string MarkerList = "";

            MarkerList += "Powtorzenia: \n";

            int i = 0;
            foreach (var marker in markers)
            {
                if (marker.Reps <= 0)
                    continue;

                if (string.IsNullOrEmpty(marker.Id))
                    continue;
                
                MarkerList +=("Name " + marker.Name
                + new String(' ', GetLongestMarkerLengthPlusOne() - marker.Name.Length)
                + "\tID: " + marker.Id + "-" + marker.Reps + "\n");
                i++;
            }

            Console.ForegroundColor = ConsoleColor.Cyan;
            Console.WriteLine(
                (new String('-', 40) + "\nIlosc kontrolek w uzyciu: " + i + "\n" +new String('-', 40)));
            Console.ResetColor();
            MarkerList += ("Wolne numery: \n");

            i = 0;
            foreach (var marker in markers)
            {
                if (marker.Reps <= 0)
                {

                    if (marker.Id != null)
                    {
                        MarkerList += ("Name "+marker.Name 
                        + new String(' ',GetLongestMarkerLengthPlusOne() - marker.Name.Length)
                        +"\tID: "+marker.Id+ "-" +marker.Reps+"\n");
                        i++;
                    }
                }
            }
            Console.ForegroundColor = ConsoleColor.Green;
            
            Console.WriteLine(new String('-', 40)+"\nIlosc nieuzywanych kontrolek: " + i + "\n" + new String('-', 40));
            Console.ResetColor();

            CreateNewFile(outputFilePath, MarkerList);

        }
        public void CreateNewFile(string path, string text)
        {
            File.WriteAllText(path, text);
        }

        public int GetLongestMarkerLengthPlusOne()
        {
            int max = 0;
            
            foreach(var marker in markers.Values)
            {
                if (max < marker.Name.Length) max = marker.Name.Length;
            }

            return max+1;

        }
        



    }
}