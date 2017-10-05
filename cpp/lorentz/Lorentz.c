/*********************************************************************************\
Fit di dati/errori lorentziana a 3 parametri
\*********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NPR	3
#define NITMAX 1000
#define VMIN 1.e-20
#define VMAX 1.e13
#define DMIN 1.e-22

#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}

void gaussj(double **, int , double **, int);
void chimin(float*, float *, float *, int, double *, double *);
double fun_teo( double,double *);
double first_der( int ,double, double * );
void nrerror( char* );
void nderror( char*, int );

double PREC;

main(int argc, char *argv[])
{
char fin[80],fou[80];
FILE *finp, *fout;
float *x,*v,*err;
double *p,*ep,*t;
int i,k,n,max;
float fmax=-1.e20,vmax=-1.e20, temp=0.;

    if(argc < 3) {
 	printf("Usage: lorentz file-in file-out [Precision]\n");
 	exit(1);
    }
    else {
        strcpy(fin, argv[1]);
        strcpy(fou, argv[2]);
    }
    if( argc == 4 ) PREC = atof(argv[3]);
    else PREC = 1.e-3;
    printf("Required relative accuracy: %f\n",PREC);

    printf("Input: %s\n",fin);
    printf("Output: %s\n",fou);

    if( (finp = fopen( fin,"r" )) == NULL ) exit(1);
    fscanf(finp,"%d\n",&n);
    printf("Number of points: %d\n",n);

    if( (x = (float*) malloc( n*sizeof(float) )) == NULL ) 
    	nderror("Error allocating x",n);
    if( (v = (float*) malloc( n*sizeof(float) )) == NULL ) 
    	nderror("Error allocating v",n);
    if( (err = (float*) malloc( n*sizeof(float) )) == NULL ) 
    	nderror("Error allocating err",n);
    for( i=0; i<n; i++ ) { 
      fscanf( finp,"%f %f %f\n",&x[i],&v[i],&err[i] ); 
      err[i] *= err[i]; 
      /* printf("%d %f %f\n",i,v[i],err[i]); */ 
    }
    fclose( finp );

    if( (p = (double*) malloc( NPR*sizeof(double) )) == NULL ) 
    	nderror("Error allocating p",NPR);
    if( (ep = (double*) malloc( NPR*sizeof(double) )) == NULL ) 
    	nderror("Error allocating ep",NPR);
    if( (t = (double*) malloc( n*sizeof(double) )) == NULL ) 
    	nderror("Error allocating t",n);

   /*PG maximum of inputs */

   for( i=0; i<n; i++ ) {
      if( v[i] > vmax ) {
        vmax = v[i];
	max = i;
      }
    }

   /*PG punto a mezza altezza (circa) oltre il max */ 

    printf("Maximum parameters: %d %f\n",max,vmax);
    i = max;
    while( i>=0 && v[i] > vmax*0.5 ) i++;

    p[0] = (double) vmax;	/* Normalisation */
    p[1] = (double) x[max];	/* Centroid */
    p[2] = (double) fabs(x[i]-x[max])*2.;	/* Width */

    /*PG massimo di fun_teo(x,p) */

    for( i=0; i<n; i++ ) {
      temp = fun_teo( (double) x[i], p );
      if( temp > fmax ) fmax = temp;
    }
    p[0] *= vmax/fmax;  /*PG => p[0] = vmax * vmax / fmax    <- (??) */

    for( i=0; i<NPR; i++ ) printf("Parameter %d initial value: %f\n",i,p[i]);

    chimin( x, v, err, n, p, ep );


/* Sezione dei risultati */
    for( i=0; i<NPR; i++ ) printf("Parameter %d final value: %f %f\n",i,p[i],ep[i]);
    for(i=0; i<n; i++ ) t[i] = (float) fun_teo( (double) x[i], p );

    if( (fout = fopen( fou,"w" )) == NULL ) exit(1);
    /* for( i=0; i<NPR; i++ ) fprintf(fout,"Parameter %d final value: %f %f\n",i,p[i],ep[i]); */ 
    for(i=0; i<n; i++ ) fprintf( fout,"%f %f %f\n", x[i], v[i], t[i] );
    fclose( fout );

/* libera memoria */
    free( p );
    free( ep );
    free( v );
    free( err );
    free( t );

}

void chimin(float x[],           /*PG ascisse punti in ingresso */
            float v[],           /*PG ordinate punti in ingresso */
	    float verr[],        /*PG errore sulle ordinate (?) */
	    int NPT,             /*PG numero di punti in ingresso */
	    double prm[],        /*PG parametri della lorentziana */
	    double perr[])       /*PG output (?)*/
{
double **mtx, **b, *d, *fvel, *dif;
double demp, div;
double vel=5.; /* Termini di velocita' */
double temp, chi2, chi_old, accuracy;
int niter, i, j, k;
int NDG;
	
/*******************************************************\
*    Inizializza .....                                  *
\*******************************************************/
	
	if( (dif = (double *) malloc( NPT*sizeof( double ) )) == NULL ) 
		nderror("Error allocating dif",NPT);
	if( (d = (double *) malloc( NPR*sizeof( double ) )) == NULL ) 
		nderror("Error allocating d",NPR);
	if( (fvel = (double *) malloc( NPR*sizeof( double ) )) == NULL ) 
		nderror("Error allocating fvel",NPR);
	if( (mtx = (double **) malloc( NPR*sizeof( double ) )) == NULL ) 
		nderror("Error allocating mtx",NPR);
	for( i=0; i<NPR; i++ ) 
	  if( (mtx[i] = (double *) malloc( NPR*sizeof( double ) )) == NULL ) 
	  	nderror("Error allocating mtx[]",NPR);
	if( (b = (double **) malloc( NPR*sizeof( double ) )) == NULL ) 
		nderror("Error allocating b",NPR);
	for( i=0; i<NPR; i++ ) 
	  if( (b[i] = (double *) malloc( 1*sizeof( double ) )) == NULL ) 
	  	nderror("Error allocating b[]",1);
	chi_old = VMAX;
	for( i=0; i<NPR; i++ ) fvel[i] = 1.;
	for( i=0; i<NPT; i++ ) dif[i] = 0.;
	
/*******************************************************\
*    ==> INIZIA AD ITERARE                              *
\*******************************************************/
  for( niter=0; niter<NITMAX; niter++) {
/*******************************************************\
*    Costruisce matrice mtx                             *
\*******************************************************/
	for( i=0; i<NPR; i++ ) {
	        b[i][0] = 0.;
		for(j=0; j<NPR; j++) mtx[i][j] = 0.; 
	}

	for( i=0; i<NPT; i++ ) {
	       for(j=0; j<NPR; j++) d[j] = first_der( j,(double) x[i],prm );
		for( j=0; j<NPR; j++ ) {
			b[j][0] += (d[j]*dif[i]/verr[i]);
			for( k=0; k<NPR; k++ ) mtx[j][k] += (d[j]*d[k]/verr[i]);
		}
	}
	
	for( i=0; i<NPR; i++ ) {
	    mtx[i][i] *= vel;
	    for( j=0; j<NPR; j++ ) mtx[i][j] *= fvel[j]; 
	}

	gaussj( mtx, NPR, b, 1);  /* Inverte matrice mtx  */

	for( i=0; i<NPR; i++ ) prm[i] += b[i][0];

/****************************************\
*    Calcola chi quadrato                *
\****************************************/
	
	chi2 = 0.;
	NDG = 0;
        for(i=0; i<NPT; i++ ) dif[i] = (float) fun_teo( (double) x[i], prm );

	for( i=0; i<NPT; i++ ) {
	  	NDG++;
		dif[i] = v[i] - dif[i];
		temp = dif[i]*dif[i]/verr[i];
		chi2 += temp;
	}
	/* chi2 /= (double) (NDG-NPR); */ 
	
	for(i=0; i<NPR; i++ ) perr[i] = (double) mtx[i][i]*vel;
	accuracy = fabs((chi_old-chi2)/chi_old);
        /* printf("Accuracy: %d %f\n",niter,accuracy); */
	if( accuracy < PREC ) break;
	chi_old = chi2;
   }
   /* for( i=0; i<NPR; i++ ) prm[i] = fabs( prm[i] ); */
   for( i=0; i<NPR; i++ ) perr[i] = sqrt( fabs(perr[i]) );
   printf("Number of iterations: %d\n",niter);
   printf("Accuracy: %f\n",accuracy);
   free( dif );
   free( d );
   free( fvel );
   free( mtx );
   free( b );
}

/*******************************************************\
*    Calcola il valore della funzione teorica nel       *
*	 punto x                                        *
\*******************************************************/		
double fun_teo( double x,double p[] )
{
double fval;
int i;

	fval = p[0]*x*x/
	   ( (x*x - p[1]*p[1])*(x*x - p[1]*p[1]) + p[2]*p[2]*x*x );
	return( fval );
}

/*******************************************************\
*    Calcola il valore delle derivate prime nel         *
*	 punto x                                        *
\*******************************************************/		
double first_der( int i_par, double x, double p[] )
{
double fval,temp;
int i,k;

     fval = 0.;
     switch( i_par ) {
       case 0:    /* Normalisation */
         fval += fun_teo( x,p )/p[0];
         break;
       case 1:
         temp = fun_teo( x,p );
         fval += 4.*temp*temp*p[1]*(x*x - p[1]*p[1])/p[0]/x/x; 
	 /*PG questa non e' la derivata di fun_teo */
         break;
       case 2:
         temp = fun_teo( x,p );
         fval -= 2.*temp*temp*p[2]/p[0];
         break;
       default:
         nderror("Unknown parameter in first_der:",i_par);
     } 
     return( fval );
     
}

void gaussj(double **a, int n, double **b, int m)
/*************************************************************************************************\
Linear equation solution by Gauss-Jordan elimination, equation (2.1.1) above. a[1..n][1..n]
is the input matrix. b[1..n][1..m] is input containing the m right-hand side vectors. On
output, a is replaced by its matrix inverse, and b is replaced by the corresponding set of solution
vectors.
\*************************************************************************************************/
{
int *indxc,*indxr,*ipiv;
int i,icol,irow,j,k,l,ll;
double big,dum,pivinv,temp;

  if( (indxc = malloc(sizeof(int)*n)) == NULL ) 
     nderror("Error allocating indxc",n); /* The integer arrays ipiv, indxr, and indxc are */
  if( (indxr = malloc(sizeof(int)*n)) == NULL ) 
    nderror("Error allocating indxr",n); /* used for bookkeeping on the pivoting. */ 
  if( (ipiv = malloc(sizeof(int)*n)) == NULL ) nderror("Error allocating ipiv",n);
  for (j=0;j<n;j++) ipiv[j]=0;
  for (i=0;i<n;i++) { /* This is the main loop over the columns to be reduced. */ 
     big=0.0;
     for (j=0;j<n;j++) /* This is the outer loop of the search for a pivot element. */
       if (ipiv[j] != 1)
         for (k=0;k<n;k++) {
           if (ipiv[k] == 0) {
             if (fabs(a[j][k]) >= big) {
               big=fabs(a[j][k]);
               irow=j;
               icol=k;
             }
           } else if (ipiv[k] > 1) nrerror("gaussj: Singular Matrix-1");
         }
     ++(ipiv[icol]);
/*************************************************************************************************\
We now have the pivot element, so we interchange rows, if needed, to put the pivot
element on the diagonal. The columns are not physically interchanged, only relabeled:
indxc[i], the column of the ith pivot element, is the ith column that is reduced, while
indxr[i] is the row in which that pivot element was originally located. If indxr[i]
!= indxc[i] there is an implied column interchange. With this form of bookkeeping, the
solution b's will end up in the correct order, and the inverse matrix will be scrambled
by columns.
\*************************************************************************************************/
     if (irow != icol) {
     	for (l=0;l<n;l++) SWAP(a[irow][l],a[icol][l])
     	for (l=0;l<m;l++) SWAP(b[irow][l],b[icol][l])
     }
     indxr[i]=irow;	   /* We are now ready to divide the pivot row by the */
     indxc[i]=icol;	   /* pivot element, located at irow and icol. */
     if (a[icol][icol] == 0.0) nrerror("gaussj: Singular Matrix-2");
     pivinv=1.0/a[icol][icol];
     a[icol][icol]=1.0;
     for (l=0;l<n;l++) a[icol][l] *= pivinv;
     for (l=0;l<m;l++) b[icol][l] *= pivinv;
     for (ll=0;ll<n;ll++) /* Next, we reduce the rows... */
       if (ll != icol) { /* ...except for the pivot one, of course. */
     	 dum=a[ll][icol];
     	 a[ll][icol]=0.0;
     	 for (l=0;l<n;l++) a[ll][l] -= a[icol][l]*dum;
     	 for (l=0;l<m;l++) b[ll][l] -= b[icol][l]*dum;
       }
  }
/*************************************************************************************************\
This is the end of the main loop over columns of the reduction. It only remains to unscram-
ble the solution in view of the column interchanges. We do this by interchanging pairs of
columns in the reverse order that the permutation was built up.
\*************************************************************************************************/
  for (l=n-1;l>=0;l--) {
    if (indxr[l] != indxc[l])
       for (k=0;k<n;k++)
          SWAP(a[k][indxr[l]],a[k][indxc[l]]);
  } /* And we are done. */
  free(ipiv);
  free(indxr);
  free(indxc);
}

void nrerror( char* str )
{
  fprintf( stderr,"%s\n",str );
  exit(1);
}

void nderror( char* str, int i)
{
  fprintf( stderr,"%s: %d\n",str,i );
  exit(1);
}
