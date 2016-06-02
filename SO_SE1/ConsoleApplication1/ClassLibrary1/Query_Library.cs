using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Query_Library
{

    public class Student
    {
        public readonly int nr;
        public readonly string name;
        public readonly int group;
        public readonly string githubId;

        public Student(int nr, String name, int group, string githubId)
        {
            this.nr = nr;
            this.name = name;
            this.group = group;
            this.githubId = githubId;
        }

        public override String ToString()
        {
            return String.Format("{0} {1} ({2}, {3})", nr, name, group, githubId);
        }
        public void Print()
        {
            Console.WriteLine(this.ToString());
        }

        public static Student Parse(string src)
        {
            string[] words = src.Split('|');
            return new Student(
                int.Parse(words[0]),
                words[1],
                int.Parse(words[2]),
                words[3]);
        }

        public override bool Equals(object stud)
        {
            Student student = (Student)stud;
            return this.name.Equals(student.name) && this.nr == student.nr && this.group == student.group && this.githubId.Equals(student.githubId);
        }
    }


    public class QueriesEager
    {
        public delegate R Mapping<T, R>(T item);
        public delegate void Consumer<T>(T item);
        public delegate bool Predicate<T>(T item);

        public static IEnumerable<R> Convert<T, R>(IEnumerable<T> src, Mapping<T, R> transf)
        {
            List<R> res = new List<R>();
            foreach (T item in src) res.Add(transf(item));
            return res;
        }

        public static IEnumerable<T> Distinct<T>(IEnumerable<T> src)
        {
            return Distinct<T>(src, s => { });
        }

        public static IEnumerable<T> Distinct<T>(IEnumerable<T> src, Consumer<T> cons)
        {

            List<T> res = new List<T>();
            foreach (T item in src)
            {
                cons(item);
                if (!res.Contains(item)) res.Add(item);
            }
            return res;
        }

        public static IEnumerable<T> Take<T>(IEnumerable<T> src, int size)
        {
            return Take<T>(src, size, s => { });
        }

        public static IEnumerable<T> Take<T>(IEnumerable<T> src, int size, Consumer<T> cons)
        {
            List<T> res = new List<T>();
            int count = 0;
            foreach (T item in src)
            {
                if (count++ >= size) break;
                cons(item);
                res.Add(item);
            }
            return res;
        }

        public static IEnumerable<T> Filter<T>(IEnumerable<T> src, Predicate<T> p)
        {
            List<T> list = new List<T>();

            foreach (T obj in src)
            {
                if (p(obj)) list.Add(obj);
            }
            return list;
        }



    }

    public class QueriesLazy
    {


        public delegate T Mapping<T>(object item);
        public delegate void Consumer<T>(T item);
        public delegate bool Predicate<T>(T item);

        public static IEnumerable<T> Convert<T, K>(IEnumerable<K> src, System.Func<object, T> transf)
        {
            return new EnumerableConverter<T, K>(src, transf);
        }

        class EnumerableConverter<T, K> : IEnumerable<T>
        {
            readonly IEnumerable<K> src;
            readonly System.Func<object, T> transf;

            public EnumerableConverter(IEnumerable<K> src, System.Func<object, T> transf)
            {
                this.src = src; this.transf = transf;
            }

            public IEnumerator<T> GetEnumerator()
            {
                return new EnumeratorConverter<T, K>(src, transf);
            }


            IEnumerator IEnumerable.GetEnumerator()
            {
                return new EnumeratorConverter<T, K>(src, transf);
            }

        }

        class EnumeratorConverter<T, K> : IEnumerator<T>
        {
            readonly IEnumerator<K> src;
            readonly System.Func<object, T> transf;
            public EnumeratorConverter(IEnumerable<K> src, System.Func<object, T> transf)
            {
                this.src = src.GetEnumerator(); this.transf = transf;
            }

            public bool MoveNext()
            {

                bool aux = src.MoveNext();
                if (aux)
                    transf(src.Current);
                return aux;
            }

            public T Current
            {
                get { return transf(src.Current); }
            }

            object IEnumerator.Current
            {
                get
                {
                    return transf(src.Current);
                }
            }

            public void Reset()
            {
                src.Reset();
            }

            public void Dispose()
            {
                src.Dispose();
            }
        }




        public static IEnumerable<T> Filter<T>(IEnumerable<T> src, System.Func<T, bool> pred)
        {
            return new EnumerableFilter<T>(src, pred);
        }

        class EnumerableFilter<T> : IEnumerable<T>
        {
            private readonly System.Func<T, bool> pred;
            readonly IEnumerable<T> src;
            public EnumerableFilter(IEnumerable<T> src, System.Func<T, bool> pred)
            {
                this.src = src;
                this.pred = pred;
            }
            public IEnumerator<T> GetEnumerator()
            {
                return new EnumeratorFilter<T>(src, pred);
            }

            IEnumerator IEnumerable.GetEnumerator()
            {
                return new EnumeratorFilter<T>(src, pred);
            }
        }

        class EnumeratorFilter<T> : IEnumerator<T>
        {
            readonly IEnumerator<T> src;
            private readonly System.Func<T, bool> pred;

            public EnumeratorFilter(IEnumerable<T> src, System.Func<T, bool> pred)
            {
                this.src = src.GetEnumerator();
                this.pred = pred;
            }

            public bool MoveNext()
            {
                while (src.MoveNext())
                {
                    if (pred(src.Current))
                        return true;
                }
                return false;
            }

            public T Current
            {
                get { return src.Current; }
            }

            object IEnumerator.Current
            {
                get
                {
                    return src.Current;
                }
            }

            public void Reset()
            {
                src.Reset();
            }

            public void Dispose()
            {
                src.Dispose();
            }
        }

        public static IEnumerable<T> Take<T>(IEnumerable<T> src, int size)
        {
            return new EnumerableMax<T>(src, size, s => { });
        }

        public static IEnumerable<T> Take<T>(IEnumerable<T> src, int size, Consumer<T> cons)
        {
            return new EnumerableMax<T>(src, size, cons);
        }

        class EnumerableMax<T> : IEnumerable<T>
        {
            private readonly Consumer<T> cons;
            readonly IEnumerable<T> src;
            readonly int max;

            public EnumerableMax(IEnumerable<T> src, int max, Consumer<T> cons)
            {
                this.src = src;
                this.max = max;
                this.cons = cons;
            }
            public IEnumerator<T> GetEnumerator()
            {
                return new EnumeratorMax<T>(src, max, cons);
            }

            IEnumerator IEnumerable.GetEnumerator()
            {
                return new EnumeratorMax<T>(src, max, cons);
            }
        }

        class EnumeratorMax<T> : IEnumerator<T>
        {
            readonly IEnumerator<T> src;
            readonly int max;
            private int count;
            private readonly Consumer<T> cons;

            public EnumeratorMax(IEnumerable<T> src, int max, Consumer<T> cons)
            {
                this.src = src.GetEnumerator();
                this.max = max;
                this.cons = cons;

            }

            public bool MoveNext()
            {
                if (count++ < max)
                {
                    bool aux = src.MoveNext();

                    return aux;
                }
                return false;
            }

            public T Current
            {
                get { cons(src.Current); return src.Current; }
            }

            object IEnumerator.Current
            {
                get
                {
                    return src.Current;
                }
            }

            public void Reset()
            {
                src.Reset();
            }

            public void Dispose()
            {
                src.Dispose();
            }
        }

        public static IEnumerable<T> Distinct<T>(IEnumerable<T> src)
        {
            return new EnumerableDistinct<T>(src, s => { });
        }

        public static IEnumerable<T> Distinct<T>(IEnumerable<T> src, Consumer<T> cons)
        {
            return new EnumerableDistinct<T>(src, cons);
        }

        class EnumerableDistinct<T> : IEnumerable<T>
        {
            private readonly Consumer<T> cons;
            readonly IEnumerable<T> src;
            public EnumerableDistinct(IEnumerable<T> src, Consumer<T> cons)
            {
                this.src = src;
                this.cons = cons;
            }
            public IEnumerator<T> GetEnumerator()
            {
                return new EnumeratorDistinct<T>(src, cons);
            }

            IEnumerator IEnumerable.GetEnumerator()
            {
                return new EnumeratorDistinct<T>(src, cons);
            }
        }

        class EnumeratorDistinct<T> : IEnumerator<T>
        {
            readonly IEnumerator<T> src;
            readonly IList<T> selected;
            private readonly Consumer<T> cons;

            public EnumeratorDistinct(IEnumerable<T> src, Consumer<T> cons)
            {
                this.src = src.GetEnumerator();
                this.selected = new List<T>();
                this.cons = cons;
            }

            public bool MoveNext()
            {
                while (src.MoveNext())
                {
                    if (!selected.Contains(src.Current))
                    {
                        cons(src.Current);
                        selected.Add(src.Current);
                        return true;
                    }
                }
                return false;
            }

            public T Current
            {
                get { return src.Current; }
            }

            object IEnumerator.Current
            {
                get
                {
                    return src.Current;
                }
            }

            public void Reset()
            {
                src.Reset();
            }

            public void Dispose()
            {
                src.Dispose();
            }
        }

    }

    public class QueriesYield
    {

        public delegate T Mapping<T>(object item);
        public delegate void Consumer<T>(T item);
        public delegate bool Predicate<T>(T item);

        public static IEnumerable<T> Convert<T, K>(IEnumerable<K> src, System.Func<object, T> transf)
        {
            foreach (K k in src)
            {
                yield return transf(k);
            }
        }


        public static IEnumerable<T> Filter<T>(IEnumerable<T> src, System.Func<T, bool> pred)
        {

            foreach (T t in src)
            {
                if (pred(t))
                    yield return t;
            }
        }

        public static IEnumerable<T> Take<T>(IEnumerable<T> src, int size)
        {
            return Take(src, size, s => { });
        }

        public static IEnumerable<T> Take<T>(IEnumerable<T> src, int size, Consumer<T> cons)
        {
            int i = 0;
            foreach (T t in src)
            {
                if (i++ < size)
                {
                    cons(t);
                    yield return t;
                }
                break;
            }
        }

        public static IEnumerable<T> Distinct<T>(IEnumerable<T> src)
        {
            return Distinct<T>(src, s => { });
        }

        public static IEnumerable<T> Distinct<T>(IEnumerable<T> src, Consumer<T> cons)
        {
            IList<T> selected = new List<T>();

            foreach (T t in src)
            {
                if (!selected.Contains(t))
                {
                    cons(t);
                    selected.Add(t);
                    yield return t;

                }

            }
        }

    }
}
