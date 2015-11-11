Musti
=====

My try of building my own lisp.

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
musti> (def {exp} (\ {x} {* x x}))
()
musti> (exp 10)
100
musti> (def {list} {1 2 3 4})
()
musti> (head list)
{1}
musti> (tail list)
{2 3 4}
musti> (join list list)
{1 2 3 4 1 2 3 4}
```
