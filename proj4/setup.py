from distutils.core import setup, Extension
import sysconfig

def main():
    CFLAGS = ['-g', '-Wall', '-std=c99', '-fopenmp', '-mavx', '-mfma', '-pthread', '-O3']
    LDFLAGS = ['-fopenmp']
    # Use the setup function we imported and set up the modules.
    # You may find this reference helpful: https://docs.python.org/3.6/extending/building.html
    # TODO: YOUR CODE
    NUMCEXT = [Extension('numc',
                         ['numc.c', 'matrix.c'],
                         extra_compile_args=CFLAGS,
                         extra_link_args=LDFLAGS,
                         )]
    setup(name = 'numc',
          version = '0.1',
          ext_modules=NUMCEXT
          )

    DUMBCEXT = [Extension('dumbc',
                         ['dumbc.c', 'dumb_matrix.c'],
                         extra_compile_args=CFLAGS,
                         extra_link_args=LDFLAGS,
                         )]
    setup(name = 'dumbc',
          version = '0.1',
          ext_modules=DUMBCEXT
          )

if __name__ == "__main__":
    main()