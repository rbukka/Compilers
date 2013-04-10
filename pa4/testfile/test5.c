/* complex switch test*/
void fun(int w, int p)
{
	 int a=9;
	 int i=1;
	 int k;
	 int m;
	 switch(i){
	  case 1: {
	    switch(a){
	      case 1:{
	        a=a+1;
	        break;		
	      }
	      case 2:{
	        switch(w) {
		  case 1: {
		     m = p * w;
	             break;
		  }
	          case 2: {
	             p = 2/w + m;
		     break;
	          }		  					
	       }		
	     }
	    default:
	       a=a+2;		  					
	    }
	   k=1;	 
	 }	 	  	
	 case 4:{
	   k=i+4;
	 }	 	  		
	 default:{
	   i = i+1;	
	   k = i+a+1;
	   i = k+20;
	 }
	 }
	 m=k+22;
	 printf("%d %d\n", m,k);
}

void main(){
  fun(1,2);
}

