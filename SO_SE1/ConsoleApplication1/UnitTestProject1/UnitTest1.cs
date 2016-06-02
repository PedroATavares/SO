using System;
using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Query_Library;
namespace UnitTestProject1
{
    [TestClass]
    public class TestsEager
    {
        [TestMethod]
        public void TestConvert()
        {
            List<Student> list = new List<Student>();
            list.Add(new Student(33017, "Edgar Filipe Lima Alves", 13, "edgaralves33017"));
            list.Add(new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira"));
            list.Add(new Student(38245, "Henrique Jacinto de Sousa Calhó", 1, "HenriqueCalho"));
            List<String> expected = new List<String>();
            expected.Add("Edgar Filipe Lima Alves");
            expected.Add("João Miguel Alves Pereira");
            expected.Add("Henrique Jacinto de Sousa Calhó");
            IEnumerable<String> result = QueriesEager.Convert(list, s => s.name);

            foreach (String l in result)
                Assert.IsTrue(expected.Contains(l));

        }

        [TestMethod]
        public void TestDistinct()
        {
            List<Student> list = new List<Student>();
            Student first = new Student(33017, "Edgar Filipe Lima Alves", 13, "edgaralves33017");
            Student second = new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira");
            Student third = new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira");
            list.Add(first);
            list.Add(second);
            list.Add(third);
            List<Student> expected = new List<Student>();
            expected.Add(first);
            expected.Add(second);
            IEnumerable<Student> result = QueriesEager.Distinct(list);
            int i = 0;
            foreach (Student l in result)
                Assert.IsTrue(l.Equals(expected[i++]));
        }


        [TestMethod]
        public void TestTake()
        {
            List<Student> list = new List<Student>();
            Student first = new Student(33017, "Edgar Filipe Lima Alves", 13, "edgaralves33017");
            Student second = new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira");
            Student third = new Student(38245, "Henrique Jacinto de Sousa Calhó", 1, "HenriqueCalho");
            list.Add(first);
            list.Add(second);
            list.Add(third);

            List<Student> expected = new List<Student>();
            expected.Add(first);
            expected.Add(second);
            IEnumerable<Student> result = QueriesEager.Take(list, 2);
            
            int i=0;
            foreach (Student l in result)
                Assert.IsTrue(l.Equals(expected[i++]));

        }

        [TestMethod]
        public void TestChain() {
            int count=0;
            List<Student> list = new List<Student>();
            Student first = new Student(33017, "Edgar Filipe Lima Alves", 13, "edgaralves33017");
            Student second = new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira");
            Student third = new Student(38245, "Edgar Filipe Lima Alves", 1, "HenriqueCalho");
            list.Add(first);
            list.Add(second);
            list.Add(third);

            IEnumerable<String> result1 = QueriesEager.Convert(list, s=>{ count++; return s.name; });
            IEnumerable<String> result2 = QueriesEager.Distinct(result1, s=> count++);
            IEnumerable<String> result3 = QueriesEager.Take(result2,1, s=> count ++);

            Assert.AreEqual(count, 7);

        }

        [TestMethod]
        public void TestFilter()
        {
            List<Student> list = new List<Student>();
            list.Add(new Student(33017, "Edgar Filipe Lima Alves", 13, "edgaralves33017"));
            list.Add(new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira"));
            list.Add(new Student(38245, "Henrique Jacinto de Sousa Calhó", 1, "HenriqueCalho"));

            List<Student> expected = new List<Student>();
            expected.Add(new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira"));
            expected.Add(new Student(38245, "Henrique Jacinto de Sousa Calhó", 1, "HenriqueCalho"));

            IEnumerable<Student> result = QueriesEager.Filter(list, x => x.nr > 34000);

            int i = 0;
            foreach (Student l in result)
                Assert.IsTrue(l.Equals(expected[i++]));

        }

   }

    [TestClass]
    public class TestsLazy
    {
        [TestMethod]
        public void TestConvert()
        {
            List<Student> list = new List<Student>();
            list.Add(new Student(33017, "Edgar Filipe Lima Alves", 13, "edgaralves33017"));
            list.Add(new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira"));
            list.Add(new Student(38245, "Henrique Jacinto de Sousa Calhó", 1, "HenriqueCalho"));
            List<String> expected = new List<String>();
            expected.Add("Edgar Filipe Lima Alves");
            expected.Add("João Miguel Alves Pereira");
            expected.Add("Henrique Jacinto de Sousa Calhó");
            IEnumerable<String> result = QueriesLazy.Convert<String,Student>(list, (object s) => { Student stud = (Student)s; return stud.name; });
            IEnumerator<String> enumerator = result.GetEnumerator();

            int i = 0;
            while (enumerator.MoveNext())
                Assert.IsTrue(enumerator.Current.Equals(expected[i++]));

        }

        [TestMethod]
        public void TestDistinct()
        {
            List<Student> list = new List<Student>();
            Student first = new Student(33017, "Edgar Filipe Lima Alves", 13, "edgaralves33017");
            Student second = new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira");
            Student third = new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira");
            list.Add(first);
            list.Add(second);
            list.Add(third);
            List<Student> expected = new List<Student>();
            expected.Add(first);
            expected.Add(second);
            IEnumerable<Student> result = QueriesLazy.Distinct(list);
            IEnumerator<Student> enumerator= result.GetEnumerator();
            
            int i = 0;
            while (enumerator.MoveNext())
                Assert.IsTrue(enumerator.Current.Equals(expected[i++]));
        }


        [TestMethod]
        public void TestTake()
        {
            List<Student> list = new List<Student>();
            Student first = new Student(33017, "Edgar Filipe Lima Alves", 13, "edgaralves33017");
            Student second = new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira");
            Student third = new Student(38245, "Henrique Jacinto de Sousa Calhó", 1, "HenriqueCalho");
            list.Add(first);
            list.Add(second);
            list.Add(third);

            List<Student> expected = new List<Student>();
            expected.Add(first);
            expected.Add(second);
            IEnumerable<Student> result = QueriesLazy.Take(list, 2);
            IEnumerator<Student> enumerator = result.GetEnumerator();

            int i = 0;
            while (enumerator.MoveNext())
                Assert.IsTrue(enumerator.Current.Equals(expected[i++]));

        }

        [TestMethod]
        public void TestFilter()
        {
            List<Student> list = new List<Student>();
            list.Add(new Student(33017, "Edgar Filipe Lima Alves", 13, "edgaralves33017"));
            list.Add(new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira"));
            list.Add(new Student(38245, "Henrique Jacinto de Sousa Calhó", 1, "HenriqueCalho"));

            List<Student> expected = new List<Student>();
            expected.Add(new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira"));
            expected.Add(new Student(38245, "Henrique Jacinto de Sousa Calhó", 1, "HenriqueCalho"));

            IEnumerable<Student> result = QueriesLazy.Filter(list, x => x.nr > 34000);
            IEnumerator<Student> enumerator = result.GetEnumerator();

            int i = 0;
            while (enumerator.MoveNext())
                Assert.IsTrue(enumerator.Current.Equals(expected[i++]));

        }

        [TestMethod]
        public void TestChain()
        {
            int count = 0;
            List<Student> list = new List<Student>();
            Student first = new Student(33017, "Edgar Filipe Lima Alves", 13, "edgaralves33017");
            Student second = new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira");
            Student third = new Student(38245, "Edgar Filipe Lima Alves", 1, "HenriqueCalho");
            list.Add(first);
            list.Add(second);
            list.Add(third);

            IEnumerable<String> result1 = QueriesLazy.Convert(list, s => { count++; Student stud = (Student)s; return stud.name; });
            IEnumerable<String> result2 = QueriesLazy.Distinct(result1, s => count++);
            IEnumerable<String> result3 = QueriesLazy.Take(result2, 1, s => count++);
            IEnumerable<String> result4 = QueriesLazy.Filter(result3, x => { count++; return x.Length > 24; });

            IEnumerator<String> enumerator1 = result1.GetEnumerator();
            IEnumerator<String> enumerator2 = result2.GetEnumerator();
            IEnumerator<String> enumerator3 = result3.GetEnumerator();
            IEnumerator<String> enumerator4 = result4.GetEnumerator();
            String str;
            while (enumerator1.MoveNext()) str= enumerator1.Current ;
            while (enumerator2.MoveNext()) str= enumerator2.Current;
            while (enumerator3.MoveNext()) str= enumerator3.Current;
            while (enumerator4.MoveNext()) str = enumerator4.Current;

            Assert.AreEqual(count, 7); // Can't make good test, don't know what is expected

        }


    }

    [TestClass]
    public class TestsYield
    {
        [TestMethod]
        public void TestConvert()
        {
            List<Student> list = new List<Student>();
            list.Add(new Student(33017, "Edgar Filipe Lima Alves", 13, "edgaralves33017"));
            list.Add(new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira"));
            list.Add(new Student(38245, "Henrique Jacinto de Sousa Calhó", 1, "HenriqueCalho"));
            List<String> expected = new List<String>();
            expected.Add("Edgar Filipe Lima Alves");
            expected.Add("João Miguel Alves Pereira");
            expected.Add("Henrique Jacinto de Sousa Calhó");
            IEnumerable<String> result = QueriesYield.Convert<String, Student>(list, (object s) => { Student stud = (Student)s; return stud.name; });
            IEnumerator<String> enumerator = result.GetEnumerator();

            int i = 0;
            while (enumerator.MoveNext())
                Assert.IsTrue(enumerator.Current.Equals(expected[i++]));

        }
        
        [TestMethod]
        public void TestFilter()
        {
            List<Student> list = new List<Student>();
            list.Add(new Student(33017, "Edgar Filipe Lima Alves", 13, "edgaralves33017"));
            list.Add(new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira"));
            list.Add(new Student(38245, "Henrique Jacinto de Sousa Calhó", 1, "HenriqueCalho"));

            List<Student> expected = new List<Student>();
            expected.Add(new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira"));
            expected.Add(new Student(38245, "Henrique Jacinto de Sousa Calhó", 1, "HenriqueCalho"));

            IEnumerable<Student> result = QueriesYield.Filter(list, x => x.nr > 34000);
            IEnumerator<Student> enumerator = result.GetEnumerator();

            int i = 0;
            while (enumerator.MoveNext())
                Assert.IsTrue(enumerator.Current.Equals(expected[i++]));

        }

        [TestMethod]
        public void TestTake()
        {
            List<Student> list = new List<Student>();
            Student first = new Student(33017, "Edgar Filipe Lima Alves", 13, "edgaralves33017");
            Student second = new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira");
            Student third = new Student(38245, "Henrique Jacinto de Sousa Calhó", 1, "HenriqueCalho");
            list.Add(first);
            list.Add(second);
            list.Add(third);

            List<Student> expected = new List<Student>();
            expected.Add(first);
            expected.Add(second);
            IEnumerable<Student> result = QueriesYield.Take(list, 2);
            IEnumerator<Student> enumerator = result.GetEnumerator();

            int i = 0;
            while (enumerator.MoveNext())
                Assert.IsTrue(enumerator.Current.Equals(expected[i++]));

        }


        [TestMethod]
        public void TestDistinct()
        {
            List<Student> list = new List<Student>();
            Student first = new Student(33017, "Edgar Filipe Lima Alves", 13, "edgaralves33017");
            Student second = new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira");
            Student third = new Student(37628, "João Miguel Alves Pereira", 1, "jomipereira");
            list.Add(first);
            list.Add(second);
            list.Add(third);
            List<Student> expected = new List<Student>();
            expected.Add(first);
            expected.Add(second);
            IEnumerable<Student> result = QueriesYield.Distinct(list);
            IEnumerator<Student> enumerator = result.GetEnumerator();

            int i = 0;
            while (enumerator.MoveNext())
                Assert.IsTrue(enumerator.Current.Equals(expected[i++]));
        }

    }
    }
