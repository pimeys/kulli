Kulli
=====

My try of building my own lisp.

Build
-----

```bash
git clone git@github.com:pimeys/kulli.git
cd kulli
mkdir build
cd build
cmake ..
make
```

Running
-------

```bash
./kulli
```

Some functionality
------------------

```lisp
kulli> (+ 1 2 (* 3 4))
15
kulli> (def {x y} 1 2)
()
kulli> (+ x y)
3
kulli> (def {exp} (\ {x} {* x x}))
()
kulli> (exp 10)
100
```
