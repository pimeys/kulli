Musti
=====

My try of building my own lisp. Musti is my other cat. This language is for
him.

![Musti the cat](http://imgur.com/Sgebbgi.jpg)

Build
-----

Install editline from your distribution (including the dev-package, if your
system uses them).

```bash
git clone git@github.com:pimeys/musti.git
cd musti
mkdir build
cd build
cmake ..
make
```

Running
-------

```bash
./musti
```

Some functionality
------------------

```lisp
musti> (+ 1 2 (* 3 4))
15
musti> (def {x y} 1 2)
()
musti> (+ x y)
3
musti> ((\ {a b} {* a b (+ a b)}) 1 2)
6
musti> (fun {fact n} {if (== n 0) {1} {* n (fact (- n 1))}})
()
musti> fact 10
3628800
musti> (def {l} (list 1 2 3 4))
()
musti> l
{1 2 3 4}
musti> (head l)
{1}
musti> (tail l)
{2 3 4}
musti> (join l l)
{1 2 3 4 1 2 3 4}
```
