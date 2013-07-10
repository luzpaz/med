# You first have to set the shell environement for medcoupling
# (see medcoupling/env.sh)

all:
	python splitdemo.py

clean:
	rm out*.vtu DomainMesh.vtu Domain.vtu *~
