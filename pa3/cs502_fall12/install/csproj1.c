#include <stdio.h>
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "cgraph.h"
#include "hashtab.h"
#include "tree-iterator.h"

//A debug flag to print debug statements if needed.
#define DEBUG 1

//Maximum number of structures in a program to store history.
#define MAX_STRUCTURES 40

//Main function which recurses through the nodes
int write_generic_node(tree node,int nspaces, bool sp);

//Function to print the array name and its dimensions
void write_array_domain (tree domain, int spc);

//Function to print a declaration like "int a"
void write_decl_name (tree node);

//Function to print the complete declaration of a function
void write_function_declaration (tree node,int spc);

//Function to write a declaration statement
void write_declaration (tree t, int spc);

//Function to write a new line
void writeNewLine();

//Function to write space into a file
void writeSpace(int spc);

//Function to write a given string into a file
void writeString(const char * str);

//An array to hold the structures already declared
int structHistory[MAX_STRUCTURES];

//global counter to iterate the above array
static int counter=0;

//variable used for  indentation
int indent = 0;

//Macro for indentation
/*
#define INDENT(SPACE)  { \
  int i; for (i = 0; i<SPACE; i++) writeSpace(1); } 
*/

#define INDENT(space) 

//File to store the final output
FILE *fp;  

//Helper to write indentation
void write_indentation()
{
	int i;
	for(i=1;i<=indent;i++)
	{
		writeString("   ");
	}
}

//Print "if" statements in a program
void print_if_statements(tree node)
{
	writeNewLine();
	indent++;	
	//if it is a statement list the ";" is handled later. 
	if(TREE_CODE(node)== STATEMENT_LIST)
	{
		write_generic_node(node,0,false);
	}
	//need to print a semicolon here.
	else
	{
		write_indentation();		
		write_generic_node(node,0,false);
		writeString(";");
		writeNewLine();
	}
	indent--;
	write_indentation();		
}

//function to check if the history array contains a structure
int contains(int addr)
{
	int i=0;
	for(i=0;i<MAX_STRUCTURES;i++)
	{
		if(addr == structHistory[i])
			return 0;
	}
	return -1;
}

//function to print declaration of a structure
void print_struct_decl (const_tree node, int spc)
{
	if(DEBUG)
		printf("%s\n", "In print_struct_decl");	

	if(contains(&TYPE_NAME (node)) == -1)
	{
	
		structHistory[counter++] = &TYPE_NAME (node);

		/* Print the name of the structure.  */
		if (TYPE_NAME (node))
		{
			INDENT (spc);
			if (TREE_CODE (node) == RECORD_TYPE)				
				writeString("struct ");		

			write_generic_node(TYPE_NAME (node), spc, false);
		}

		/* Print the contents of the structure.  */
		writeNewLine();

		INDENT (spc);
		
		writeString("{");
	
		
		writeNewLine();	

		/* Print the fields of the structure.  */
		{
			tree tmp;
			tmp = TYPE_FIELDS (node);
			while (tmp)
			{
				if (TREE_TYPE (tmp) != node  && (TREE_CODE (TREE_TYPE (tmp)) != POINTER_TYPE || TREE_TYPE (TREE_TYPE (tmp)) != node))
				{
					write_declaration (tmp, spc+2);					
					writeNewLine();
		  		}
				tmp = DECL_CHAIN (tmp);
			}
		}
		INDENT (spc);
		writeString("}");
	}
	else
	{
		/* Print the name of the structure.  */
		if (TYPE_NAME (node))
		{
			//INDENT (spc);
			if (TREE_CODE (node) == RECORD_TYPE)				
				writeString("struct ");			

			write_generic_node(TYPE_NAME (node), spc, false);
		}
	}

}

//Helper function to write integer into the output file.
void writeInteger(int tWrite)
{
	fprintf(fp,"%d",tWrite);
}


//Helper function to write space into the output file.
void writeSpace(int spc)
{
	int i;
	for(i=0;i<spc;i++)
		fprintf(fp,"%s"," ");
}

//Helper function to write a new line to the output file.
void writeNewLine()
{
	fprintf(fp,"%s","\n");
}

//Helper function to write a given string to the output file.
void writeString(const char * str)
{
	fprintf(fp,"%s",str);
}


/* Print the declaration of a variable.  */
void write_declaration (tree t, int spc)
{
	INDENT (spc);
	writeSpace(spc);	

	if (TREE_CODE (t) == TYPE_DECL)
		writeString("typedef ");

	if (CODE_CONTAINS_STRUCT (TREE_CODE (t), TS_DECL_WRTL) && DECL_REGISTER (t))
		writeString("register ");

	if (TREE_PUBLIC (t) && DECL_EXTERNAL (t))
		writeString("extern ");
	else if (TREE_STATIC (t))
		writeString("static ");

	/* Print the type and name.  */
	if (TREE_TYPE (t) && TREE_CODE (TREE_TYPE (t)) == ARRAY_TYPE)
	{
		tree tmp;

		/* Print array's type.  */
		tmp = TREE_TYPE (t);
		while (TREE_CODE (TREE_TYPE (tmp)) == ARRAY_TYPE)
			tmp = TREE_TYPE (tmp);

		write_generic_node(TREE_TYPE (tmp), spc, false);

		/* Print variable's name.  */
		
		writeSpace(1);
		write_generic_node(t, spc,false);

		/* Print the dimensions.  */
		tmp = TREE_TYPE (t);
		while (TREE_CODE (tmp) == ARRAY_TYPE)
		{
			write_array_domain (TYPE_DOMAIN (tmp), spc);
			tmp = TREE_TYPE (tmp);
		}
	}
	else if (TREE_CODE (t) == FUNCTION_DECL)
	{
		write_generic_node (TREE_TYPE (TREE_TYPE (t)), spc, false);
		writeSpace(1);

		write_decl_name(t);
		write_function_declaration (TREE_TYPE (t), spc);
	}
	else
	{
		/* Print type declaration.  */
		write_generic_node(TREE_TYPE (t), spc, false);

		/* Print variable's name.  */
		writeSpace(1);
		write_generic_node (t, spc, false);		
	}


	if (TREE_CODE (t) == VAR_DECL && DECL_HARD_REGISTER (t))
	{
		writeString("___asm___");
		writeString("(");
		write_generic_node(DECL_ASSEMBLER_NAME (t), spc, false);
		writeString(")");
	}

  /* The initial value of a function serves to determine whether the function
     is declared or defined.  So the following does not apply to function
     nodes.  */
	if (TREE_CODE (t) != FUNCTION_DECL)
	{
		/* Print the initial value.  */
		if (DECL_INITIAL (t))
		{
			writeSpace(1);
			writeString("=");
			writeSpace(1);
			write_generic_node ( DECL_INITIAL (t), spc, false);
		}
	}

	if (TREE_CODE (t) == VAR_DECL && DECL_HAS_VALUE_EXPR_P (t))
	{
		writeString(" [value-expr: ");
		write_generic_node(DECL_VALUE_EXPR (t), spc, false);
		writeString("]");
	}

	writeString(";");
}



//Helper function to print characters into a file which have escape property
void ePrint(const char *str)
{
	if (str == NULL)
		return;

	while (*str)
	{
		switch (str[0])
		{
			case '\b':
				fprintf(fp,"\\b");				
				break;

			case '\f':
				fprintf(fp,"\\f");
				break;

			case '\n':
				fprintf(fp,"\\n");
				break;

			case '\r':
				fprintf(fp,"\\r");
				break;

			case '\t':
				fprintf(fp,"\\t");
				break;

			case '\v':
				fprintf(fp,"\\v");
				break;

			case '\\':
				fprintf(fp,"\\\\");
				break;

			case '\"':
				fprintf(fp,"\\\"");
				break;
	
			case '\'':
				fprintf(fp,"\\'");
				break;

			default:
				fprintf(fp,"%c",str[0]);
				break;
		}
		str++;
	}
}

//Helper funcion to check the data type of a node.
int check_type(tree node)
{
	switch(TREE_CODE(node))
	{
		case INTEGER_TYPE:
			if(TYPE_PRECISION(node)==8)
				fprintf(fp,"%s","char ");
			else
				fprintf(fp,"%s","int ");		
			if(DEBUG)
				printf("Returns Integer\n");
			break;
		case VOID_TYPE:
			fprintf(fp,"%s","void ");
			if(DEBUG)
				printf("Returns Void\n");
			break;
		case REAL_TYPE:
			fprintf(fp,"%s","float ");
			if(DEBUG)
				printf("Returns Real\n");
			break;
	}
	return 0;
}

//Helper function to write the name of a variable or function.
void write_decl_name (tree node)
{
  if (DECL_NAME (node))
  {
	fprintf (fp,"%s",IDENTIFIER_POINTER(DECL_NAME (node)));
  } 
}

//Helper function to write a function name
void write_function_name (tree node)
{
  if (TREE_CODE (node) == NOP_EXPR)
    node = TREE_OPERAND (node, 0);
  write_decl_name (node);
}

/* Write the function declaration.  NODE is the FUNCTION_TYPE.  BUFFER, SPC and
   FLAGS are as in write_generic_node.  */

void write_function_declaration (tree node,int spc)
{
	bool wrote_arg = false;
	tree arg;

	writeSpace(1);	

	writeString("(");

	/* Print the argument types.  */
	arg = TYPE_ARG_TYPES (node);
	while (arg && arg != void_list_node && arg != error_mark_node)
	{
		if (wrote_arg)
		{
			writeString(",");			
			writeSpace(1);
		}
		wrote_arg = true;
		write_generic_node (TREE_VALUE (arg), spc,false);
		arg = TREE_CHAIN (arg);
	}

	/* Drop the trailing void_type_node if we had any previous argument.  */
	if (arg == void_list_node && !wrote_arg)		
		writeString("void");
	/* Properly dump vararg function types.  */
	else if (!arg && wrote_arg)
		writeString(", ...");
	/* Avoid printing any arg for unprototyped functions.  */
	writeString(")");
}


/* Write the domain associated with an array.  */

void write_array_domain (tree domain, int spc)
{
	writeString("[");

	if(domain)
	{
		tree min = TYPE_MIN_VALUE (domain);
		tree max = TYPE_MAX_VALUE (domain);
	
		if (min && max  && integer_zerop (min) && host_integerp (max, 0))
			writeInteger(TREE_INT_CST_LOW (max) + 1);			
		else
		{
			if (min)
				write_generic_node(min,spc,false);
			writeString(":");

			if (max)
				write_generic_node(max,spc,false);	
		}
	}
	else
		writeString("<unknown");
	writeString("]");	
}

//Helper function which process each function in the program. It takes a function node as an argument and also uses nspaces and sp to align the code.
int write_generic_node(tree node,int nspaces, bool sp)
{
	printf("TREE CODE: %s\n",tree_code_name[(int)TREE_CODE(node)]);	

	//temporary variable for getting function parameters
	tree parameters;

	//temporary variable for getting function blocks
	tree blocks;

	//temporary variable for getting function variables
	tree variables;
	
	//temporary variable for getting the return type
	tree returnType;
	
	//temporary variable to store the saved tree and extract statement list
	tree savedTree;
	
	//temporary variable to store the body of the saved tree
	tree savedTreeBody;
	
	//temporary variable to iterate through the statement list
	tree_stmt_iterator si;
	
	//temporary variable to store the statement list
	tree statementList;
	
	//temporary variable to store operands
	tree op1;
	
	//temporary variable to store operands
	tree op2;

	//temporary variable to store operands
	tree op0;

	//temporary string
	const char *str;

	//used to print semicolon
	bool expr;

	if (node == NULL_TREE)
		return nspaces;

	expr = EXPR_P (node);

	//indentation
	if(sp)
	{
		writeNewLine();
		writeSpace(nspaces);
	}

	//Represent conversion from one pointer to another	
	if(TREE_CODE(node)==NOP_EXPR)
	{
		if(DEBUG)			
				printf("%s\n", " IN NOP_EXPR " );
	
		write_generic_node(TREE_OPERAND(node, 0),0,false);
	}

	 //Handle a float value
	 if(TREE_CODE(node)==REAL_CST)
	 {
		char string[100];
		REAL_VALUE_TYPE d;
		d = TREE_REAL_CST (node);
		real_to_decimal(string,&d,sizeof(string),0,1);
		writeString(string);
	 }

	//Handle a interger value	
	if(TREE_CODE(node)==INTEGER_CST)
	{
		unsigned HOST_WIDE_INT low = TREE_INT_CST_LOW (node);
		HOST_WIDE_INT high = TREE_INT_CST_HIGH (node);
	
		if (tree_int_cst_sgn(node) < 0)
		{
			writeString("-");
			high = ~high + !low;
			low = -low;
		}
		if((unsigned HOST_WIDE_INT) high==0)
			fprintf (fp,"%d",low);
		else
			fprintf (fp,"%d%d",(unsigned HOST_WIDE_INT)high,low);
	}

	//Handle a integer type
	if(TREE_CODE(node)==INTEGER_TYPE)
	{
		if(TYPE_PRECISION(node)==8)
			writeString("char");
		else
			writeString("int");
	}

	//Handle a string type
	if(TREE_CODE(node)==STRING_CST)
	{
		if(DEBUG)			
				printf("%s\n", " IN STRING_CST " );		

		writeString("\"");
		ePrint(TREE_STRING_POINTER(node));
		writeString("\"");
	}


	//Handle a float type
	if(TREE_CODE(node)==REAL_TYPE)
	{
		writeString("float");
	}

	//Handle a boolean type
	if(TREE_CODE(node)==BOOLEAN_TYPE)
	{
		writeString("bool");
	}

	//Handle a void type
	if(TREE_CODE(node)==VOID_TYPE)
	{
		writeString("void");
	}

	//Print a name of identifier
	if(TREE_CODE(node)==IDENTIFIER_NODE)
	{
		fprintf(fp,"%s",IDENTIFIER_POINTER(node));
		//printf("%s\n",IDENTIFIER_POINTER(node));
	}

	//Handle a function.
	if(TREE_CODE(node) == FUNCTION_DECL)
	{
		if(DEBUG)			
				printf("%s\n", " IN FUNCTION_DECL " );
		
		char *c = "\0";
		tree args;
		write_generic_node(TREE_TYPE(DECL_RESULT(node)),nspaces,true);
		writeSpace(1);
		write_generic_node(DECL_NAME(node),0,false);
		writeString("(");
	
		args = DECL_ARGUMENTS(node);
	
	       	while(args)
	       	{	
			fprintf(fp,"%s",c);
			c = ", ";	
			write_generic_node(TREE_TYPE(args),0,false);
			writeSpace(1);
			write_generic_node(DECL_NAME(args),0,false);
			args = TREE_CHAIN(args);
	       	}

		writeString(") {");	
		
		write_generic_node(BIND_EXPR_BODY(DECL_SAVED_TREE(node)),nspaces+2,false);
		
		if(TREE_CODE(BIND_EXPR_BODY(DECL_SAVED_TREE(node))) != STATEMENT_LIST && TREE_CODE(BIND_EXPR_BODY(DECL_SAVED_TREE(node))) != RETURN_EXPR)
		{
			writeString(";");	
		}	
		
		writeSpace(nspaces);
		writeNewLine();
		writeString("}");

	}

	//Handle statements inside a function
	if(TREE_CODE(node) == STATEMENT_LIST)
	{
		tree_stmt_iterator statementIterator;
		for (statementIterator = tsi_start(node); !tsi_end_p(statementIterator); tsi_next(&statementIterator))
		{
			if(TREE_CODE(tsi_stmt(statementIterator))!=RETURN_EXPR)
			{
				write_generic_node(tsi_stmt(statementIterator), nspaces, true);
				writeString(";");
			}
			else
			{
				write_generic_node(tsi_stmt(statementIterator), nspaces, false);				
			}
		}

	}

		
		//Handle a return expression
		if(TREE_CODE(node) == RETURN_EXPR)
		{
			writeNewLine();
			writeSpace(nspaces);
			writeString("return");
			op1 = TREE_OPERAND (node,0);
			if (op1)
			{	
				writeSpace(1);
				if (TREE_CODE (op1) == MODIFY_EXPR)
					write_generic_node(TREE_OPERAND(op1, 1),0,false);
				else
					write_generic_node(op1,0,false);
			}
			writeString(";");			
		}	


	//Handle a declaration expression
	if(TREE_CODE(node)==DECL_EXPR)
	{
		if(DEBUG)			
				printf("%s\n", " IN DECL EXPR " );
	
		writeVarDecl(DECL_EXPR_DECL (node));
	}

	
	//Handle function calls like printf
	if(TREE_CODE(node) == CALL_EXPR)
	{
			if(DEBUG)			
				printf("%s\n", " IN CALL EXPR " );

			write_generic_node(DECL_NAME(DECL_NAME(CALL_EXPR_FN(node))),0,false);
			writeString("(");

			char *c = "\0";
			tree arg;
			call_expr_arg_iterator iter;
			FOR_EACH_CALL_EXPR_ARG (arg, iter, node)
			{
				fprintf(fp,"%s",c);
				c = ", ";
				write_generic_node(arg, 0, false);
			}
		writeString(")");	
	
	}

	//The below code handles all the expressions like addition, negation, modify etc.,
	
	if(TREE_CODE(node) == MODIFY_EXPR || TREE_CODE(node) == INIT_EXPR)
	{
		write_generic_node(TREE_OPERAND(node,0),nspaces,false);
		writeString(" = ");
		write_generic_node(TREE_OPERAND(node,1),nspaces,false);   			
	}

	if(TREE_CODE(node) == NEGATE_EXPR)
	{
		op1 = TREE_OPERAND(node,0);
		writeString("!");


		if (op_prio(op1) <= op_prio(node))
		{
			writeString(" (");
			write_generic_node(op1, nspaces, false);
			writeString(")");
		}
		else
			write_generic_node(op1, nspaces, false); 			
	}

	if(TREE_CODE(node) == INDIRECT_REF)
	{
		op1 = TREE_OPERAND(node,0);
		writeString("*");


		if (op_prio(op1) <= op_prio(node))
		{
			writeString(" (");
			write_generic_node(op1, nspaces, false);
			writeString(")");
		}
		else
			write_generic_node(op1, nspaces, false); 			
	}
	

	if(TREE_CODE(node) == ADDR_EXPR)
	{
		if (TREE_CODE (node) == ADDR_EXPR && (TREE_CODE (TREE_OPERAND (node, 0)) == STRING_CST
			|| TREE_CODE (TREE_OPERAND (node, 0)) == FUNCTION_DECL))
		;
		else
			writeString(op_symbol_code(TREE_CODE (node)));

		if (op_prio (TREE_OPERAND(node, 0)) < op_prio(node))
		{
			writeString(" (");
			write_generic_node(TREE_OPERAND (node, 0), 0, false);
			writeString(")");
		}
		else
			write_generic_node(TREE_OPERAND (node, 0), 0, false);
	}


	if(TREE_CODE(node) == TRUTH_OR_EXPR || TREE_CODE(node) == TRUTH_ORIF_EXPR || TREE_CODE(node) == TRUTH_AND_EXPR)
	{
		op1 = TREE_OPERAND(node,0);
		op2 = TREE_OPERAND(node,1);
		/* When the operands are expressions with less priority, keep semantics of the tree representation.  */
		if (op_prio(op1) <= op_prio(node))
		{
			writeString(" (");
			write_generic_node(op1, nspaces, false);
			writeString(")");
		}
		else
			write_generic_node(op1, nspaces, false);

			writeSpace(1);
			writeString(op_symbol_code(TREE_CODE (node)));
			writeSpace(1);
			//TODO hanlde op_prio
			/* When the operands are expressions with less priority, keep semantics of the tree representation.  */
			if (op_prio(op2) <= op_prio(node))
			{
				writeString("(");
				write_generic_node(op2, nspaces, false);
				writeString("(");
			}
			else
				write_generic_node(op2, nspaces, false);
	}
	
	if(TREE_CODE(node) == TRUTH_ANDIF_EXPR || TREE_CODE(node) == MULT_EXPR || TREE_CODE(node) == PLUS_EXPR || TREE_CODE(node) == MINUS_EXPR)
	{
		op1 = TREE_OPERAND(node,0);
		op2 = TREE_OPERAND(node,1);
		/* When the operands are expressions with less priority, keep semantics of the tree representation.  */
		if (op_prio(op1) <= op_prio(node))
		{
			writeString("(");
			write_generic_node(op1, nspaces, false);
			writeString(")");
		}
		else
			write_generic_node(op1, nspaces, false);

			writeSpace(1);
			writeString(op_symbol_code(TREE_CODE (node)));
			writeSpace(1);
			//TODO hanlde op_prio
			/* When the operands are expressions with less priority, keep semantics of the tree representation.  */
			if (op_prio(op2) <= op_prio(node))
			{
				writeString("(");
				write_generic_node(op2, nspaces, false);
				writeString("(");
			}
			else
				write_generic_node(op2, nspaces, false);
	}

	if(TREE_CODE(node) == TRUNC_DIV_EXPR || TREE_CODE(node) == CEIL_DIV_EXPR || TREE_CODE(node) == FLOOR_DIV_EXPR || TREE_CODE(node) == ROUND_DIV_EXPR)
	{
		op1 = TREE_OPERAND(node,0);
		op2 = TREE_OPERAND(node,1);
		/* When the operands are expressions with less priority, keep semantics of the tree representation.  */
		if (op_prio(op1) <= op_prio(node))
		{
			writeString("(");
			write_generic_node(op1, nspaces, false);
			writeString(")");
		}
		else
			write_generic_node(op1, nspaces, false);

			writeSpace(1);
			writeString(op_symbol_code(TREE_CODE (node)));
			writeSpace(1);
			//TODO hanlde op_prio
			/* When the operands are expressions with less priority, keep semantics of the tree representation.  */
			if (op_prio(op2) <= op_prio(node))
			{
				writeString("(");
				write_generic_node(op2, nspaces, false);
				writeString("(");
			}
			else
				write_generic_node(op2, nspaces, false);
	}
	
	if(TREE_CODE(node) == RDIV_EXPR || TREE_CODE(node) == EXACT_DIV_EXPR || TREE_CODE(node) == LT_EXPR || TREE_CODE(node) == LE_EXPR)
	{
		op1 = TREE_OPERAND(node,0);
		op2 = TREE_OPERAND(node,1);
		/* When the operands are expressions with less priority, keep semantics of the tree representation.  */
		if (op_prio(op1) <= op_prio(node))
		{
			writeString("(");
			write_generic_node(op1, nspaces, false);
			writeString(")");
		}
		else
			write_generic_node(op1, nspaces, false);

			writeSpace(1);
			writeString(op_symbol_code(TREE_CODE (node)));
			writeSpace(1);
			//TODO hanlde op_prio
			/* When the operands are expressions with less priority, keep semantics of the tree representation.  */
			if (op_prio(op2) <= op_prio(node))
			{
				writeString("(");
				write_generic_node(op2, nspaces, false);
				writeString("(");
			}
			else
				write_generic_node(op2, nspaces, false);
	}

	if(TREE_CODE(node) == GT_EXPR || TREE_CODE(node) == GE_EXPR || TREE_CODE(node) == EQ_EXPR || TREE_CODE(node) == NE_EXPR)
	{
		op1 = TREE_OPERAND(node,0);
		op2 = TREE_OPERAND(node,1);
		/* When the operands are expressions with less priority, keep semantics of the tree representation.  */
		if (op_prio(op1) <= op_prio(node))
		{
			writeString("(");
			write_generic_node(op1, nspaces, false);
			writeString(")");
		}
		else
			write_generic_node(op1, nspaces, false);

			writeSpace(1);
			writeString(op_symbol_code(TREE_CODE (node)));
			writeSpace(1);
			//TODO hanlde op_prio
			/* When the operands are expressions with less priority, keep semantics of the tree representation.  */
			if (op_prio(op2) <= op_prio(node))
			{
				writeString("(");
				write_generic_node(op2, nspaces, false);
				writeString("(");
			}
			else
				write_generic_node(op2, nspaces, false);
	}

	if(TREE_CODE(node) == PARM_DECL || TREE_CODE(node) == VAR_DECL || TREE_CODE(node) == FIELD_DECL)
	{
		if(DEBUG)			
				printf("%s\n", " IN PARM_DECL or VAR_DECL EXPR " );

		fprintf(fp,"%s",IDENTIFIER_POINTER(DECL_NAME(node)));
		//printf("%s\n",IDENTIFIER_POINTER(DECL_NAME(node)));
	}
	
	//MileStone 2
	//Handle pointer or reference type
	if(TREE_CODE(node) == POINTER_TYPE || TREE_CODE(node) == REFERENCE_TYPE)
	{      	
      	      	str = (TREE_CODE (node) == POINTER_TYPE ? "*" : "&");

		//Handle different types for which pointer may point to
		if (TREE_TYPE (node) == NULL)
	        {
			writeString(str);
			writeString("<null type>");
	        }
		else if (TREE_CODE (TREE_TYPE (node)) == FUNCTION_TYPE)
	        {
			tree fnode = TREE_TYPE (node);
			write_generic_node(TREE_TYPE (fnode), nspaces, false);				

			
			writeSpace(1);
			
			
			writeString("(");		

			
			writeString(str);

			if (TYPE_NAME (node) && DECL_NAME (TYPE_NAME (node)))				
				write_decl_name(TYPE_NAME(node));
			
			writeString(")");		
			write_function_declaration (fnode, nspaces);
		}
		else
		{
			unsigned int quals = TYPE_QUALS (node);
			write_generic_node(TREE_TYPE (node), nspaces, false);		

			writeSpace(1);

			writeString(str);
			
			if (quals & TYPE_QUAL_CONST)
				writeString(" const");
			if (quals & TYPE_QUAL_VOLATILE)
				writeString(" volatile");	
			if (quals & TYPE_QUAL_RESTRICT)
				writeString(" restrict");	
			
		}
	}

	//Handle a loop expression if it does not go to if or goto
	if(TREE_CODE(node) == LOOP_EXPR)
	{
		if(DEBUG)
			printf("%s\n","In LOOP_EXPR");

		
		writeString("while (1)");
		
		{
			
			writeNewLine();
			writeSpace(nspaces+2);

			
			writeString("{");


			writeNewLine();
			writeSpace(nspaces+2);

			
			write_generic_node(LOOP_EXPR_BODY (node), nspaces+4, true);

			
			writeNewLine();
			writeSpace(nspaces+2);
	
			
			writeString("}");
		}
	      expr = false;
	}

	//Handle an array declaration
	if(TREE_CODE(node) == ARRAY_TYPE)
	{ 
		tree tmp;

		// Print the innermost component type.  
		for (tmp = TREE_TYPE (node); TREE_CODE (tmp) == ARRAY_TYPE;
			tmp = TREE_TYPE (tmp))
		;
		write_generic_node (tmp, nspaces, false);

		/* Print the dimensions.  */
		for (tmp = node; TREE_CODE (tmp) == ARRAY_TYPE; tmp = TREE_TYPE (tmp))			
			write_array_domain (TYPE_DOMAIN (tmp), nspaces);
	}

	//Handle struct type
	if(TREE_CODE(node) == RECORD_TYPE)
	{
		if(DEBUG)
			printf("In RECORD_TYPE \n" );		

		unsigned int quals = TYPE_QUALS (node);
		if (quals & TYPE_QUAL_CONST)			
			writeString("const ");

		if (quals & TYPE_QUAL_VOLATILE)			
			writeString("volatile ");

		//print structure declaration
		print_struct_decl (node, nspaces);

		
	}

	//Handle referencing using a structure in particular
	if(TREE_CODE(node) == COMPONENT_REF)
	{	
		op0 = TREE_OPERAND (node, 0);
		str = ".";
		if (op0 && (TREE_CODE (op0) == INDIRECT_REF || (TREE_CODE (op0) == MEM_REF
		  && TREE_CODE (TREE_OPERAND (op0, 0)) != ADDR_EXPR
		  && integer_zerop (TREE_OPERAND (op0, 1))
		  && TREE_CODE (TREE_OPERAND (op0, 0)) != INTEGER_CST
		  && TREE_TYPE (TREE_OPERAND (op0, 0)) != NULL_TREE
		  && (TREE_TYPE (TREE_TYPE (TREE_OPERAND (op0, 0)))
		      == TREE_TYPE (TREE_TYPE (TREE_OPERAND (op0, 1))))
		  && (TYPE_MODE (TREE_TYPE (TREE_OPERAND (op0, 0)))
		      == TYPE_MODE (TREE_TYPE (TREE_OPERAND (op0, 1))))
		  && (TYPE_REF_CAN_ALIAS_ALL (TREE_TYPE (TREE_OPERAND (op0, 0)))
		      == TYPE_REF_CAN_ALIAS_ALL (TREE_TYPE (TREE_OPERAND (op0, 1))))
		  && (TYPE_MAIN_VARIANT (TREE_TYPE (op0))
		      == TYPE_MAIN_VARIANT
		          (TREE_TYPE (TREE_TYPE (TREE_OPERAND (op0, 1))))))))
		{
			  op0 = TREE_OPERAND (op0, 0);
			  str = "->";
		}
		if (op_prio (op0) < op_prio (node))			
			writeString("(");

		write_generic_node(op0,nspaces,false);

		if (op_prio (op0) < op_prio (node))			
			writeString(")");

		writeString(str);
		
		write_generic_node( TREE_OPERAND (node, 1), nspaces, false);

		op0 = component_ref_field_offset (node);

		if (op0 && TREE_CODE (op0) != INTEGER_CST)
		{
			writeString("{off: ");			
			write_generic_node(op0,nspaces,false);
			writeString("}");
		}
	}
	
	//Handle reference to an array
	if(TREE_CODE(node) == ARRAY_REF || TREE_CODE(node) == ARRAY_RANGE_REF)
	{	
		op0 = TREE_OPERAND (node, 0);
		if (op_prio (op0) < op_prio (node))
			writeString("(");

		write_generic_node(op0,nspaces,false);
		
		if (op_prio (op0) < op_prio (node))			
			writeString(")");
		
		writeString("[");

//		write_generic_node(TREE_OPERAND (node, 1), nspaces,  false);

		printf("%s %s\n","Index in array",IDENTIFIER_POINTER(DECL_NAME(TREE_OPERAND(node,1))));

		if (TREE_CODE (node) == ARRAY_RANGE_REF)			
			writeString(" ...");

		writeString("]");

		op0 = array_ref_low_bound (node);
		op1 = array_ref_element_size (node);

		if (!integer_zerop (op0) || TREE_OPERAND (node, 2)  || TREE_OPERAND (node, 3))		{
			
			writeString("{lb: ");

			write_generic_node(op0,nspaces,false);

			writeString(" sz: ");

			write_generic_node(op1,nspaces,false);

			writeString("}");
		}
	}

	//Handle a constructor for an array
	if(TREE_CODE(node) == CONSTRUCTOR)
	{
		unsigned HOST_WIDE_INT ix;
		tree field, val;
		bool is_struct_init = false;
		bool is_array_init = false;
		double_int curidx = double_int_zero;
		writeString("{");

		if (TREE_CLOBBER_P (node))
			writeString("CLOBBER");
		else if (TREE_CODE (TREE_TYPE (node)) == RECORD_TYPE
			|| TREE_CODE (TREE_TYPE (node)) == UNION_TYPE)
			is_struct_init = true;
		else if (TREE_CODE (TREE_TYPE (node)) == ARRAY_TYPE
		 && TYPE_DOMAIN (TREE_TYPE (node))
		 && TYPE_MIN_VALUE (TYPE_DOMAIN (TREE_TYPE (node)))
		 && TREE_CODE (TYPE_MIN_VALUE (TYPE_DOMAIN (TREE_TYPE (node))))
		    == INTEGER_CST)
		{
			tree minv = TYPE_MIN_VALUE (TYPE_DOMAIN (TREE_TYPE (node)));
			is_array_init = true;
			curidx = tree_to_double_int (minv);
		}

		FOR_EACH_CONSTRUCTOR_ELT (CONSTRUCTOR_ELTS (node), ix, field, val)
		{
			if (field)
			{
				if (is_struct_init)
				{
					writeString(".");					
					write_generic_node(field,nspaces,false);
					writeString("=");
				}
				else if (is_array_init && (TREE_CODE (field) != INTEGER_CST || !double_int_equal_p (tree_to_double_int (field),
						     curidx)))
				{

					writeString("[");
					if (TREE_CODE (field) == RANGE_EXPR)
					{
						write_generic_node(TREE_OPERAND (field, 0), nspaces, false);
						writeString(" ... ");
						write_generic_node(TREE_OPERAND (field, 1), nspaces,false);

						if (TREE_CODE (TREE_OPERAND (field, 1)) == INTEGER_CST)
							curidx = tree_to_double_int (TREE_OPERAND (field, 1));
					}
					else
						write_generic_node(field,nspaces,false);

					if (TREE_CODE (field) == INTEGER_CST)
						curidx = tree_to_double_int (field);

					writeString("]=");
				}
			}
				if (is_array_init)
					curidx = double_int_add (curidx, double_int_one);
				if (val && TREE_CODE (val) == ADDR_EXPR)
					if (TREE_CODE (TREE_OPERAND (val, 0)) == FUNCTION_DECL)
						val = TREE_OPERAND (val, 0);
		
				if (val && TREE_CODE (val) == FUNCTION_DECL)					
					write_decl_name(val);
				else
					write_generic_node(val,nspaces,false);

				if (ix != VEC_length (constructor_elt, CONSTRUCTOR_ELTS (node)) - 1)
				{
					writeString(",");
					writeSpace(1);
				}
		}
		writeString("}");
	}

	//Handle the pointer type allocation of a struct
	if(TREE_CODE(node) == COMPOUND_EXPR)
	{
		tree *tp;
		
		{
			write_generic_node(TREE_OPERAND (node, 0),nspaces,false);

			
			{
			
				writeString(",");
			
				writeSpace(1);
			}

			for (tp = &TREE_OPERAND (node, 1);
			TREE_CODE (*tp) == COMPOUND_EXPR;
			tp = &TREE_OPERAND (*tp, 1))
			{
				
				write_generic_node(TREE_OPERAND (*tp, 0),nspaces,false);

				
				{
				
					writeString(",");
					
					writeSpace(1);
				}
			}
			write_generic_node(*tp,nspaces,false);
		}
	}

	//Handle "if"	
	if(TREE_CODE(node) == COND_EXPR)
	{
		writeString("if (");
		write_generic_node (COND_EXPR_COND (node),0,false);
		writeString(")");
		writeString("{");
		print_if_statements(COND_EXPR_THEN(node));
		writeString("}");
		if (COND_EXPR_ELSE (node) && !IS_EMPTY_STMT (COND_EXPR_ELSE (node)))
		{
			writeString("else {");
			print_if_statements(COND_EXPR_ELSE(node));
			writeString("}");
		}
	}

	//Handle Bind Expression which contains blocks inside "if" or just blocks
	if(TREE_CODE(node) == BIND_EXPR)
	{
		writeString("{");
		
		{
			if (BIND_EXPR_VARS (node))
			{
				
				writeNewLine();
				for (op0 = BIND_EXPR_VARS (node); op0; op0 = DECL_CHAIN (op0))
				{
					writeNewLine();
				}
			}

			writeNewLine();
			writeSpace(nspaces + 2);

			write_generic_node(BIND_EXPR_BODY (node), nspaces+2, true);
			writeNewLine();
			writeSpace(nspaces);
			writeString("}");
		}
		expr = false;
	}
	
	//Handle switch case	
	if(TREE_CODE(node) == SWITCH_EXPR)
	{
		writeString("switch (");		
		write_generic_node(SWITCH_COND(node), nspaces, false);
		writeString(")");
		{

			writeNewLine();
			writeSpace(nspaces+2);
			writeString("{");

			if (SWITCH_BODY (node))
			{

				writeNewLine();
				writeSpace(nspaces+2);
				write_generic_node(SWITCH_BODY (node), nspaces+4,true);
			}
			else
			{
				tree vec = SWITCH_LABELS (node);
				size_t i, n = TREE_VEC_LENGTH (vec);
				for (i = 0; i < n; ++i)
				{
					tree elt = TREE_VEC_ELT (vec, i);
					
					writeNewLine();
					writeSpace(nspaces+4);

					if (elt)
					{
						
						write_generic_node(elt, nspaces+4,false);
						
						writeString(" goto ");

						
						write_generic_node(CASE_LABEL (elt), nspaces+4, true);
						
						writeString(";");
					}
					else
						
						writeString("case ???: goto ???;");
				}
	    		}	

			
			writeNewLine();
			writeSpace(nspaces+2);			
			writeString("}");
		}
      		expr = false;
	}

	//Handle case expression
	if(TREE_CODE(node) == CASE_LABEL_EXPR)
	{
		if (CASE_LOW (node) && CASE_HIGH (node))
		{

			writeString(" case ");
			write_generic_node(CASE_LOW (node), nspaces, false);
			writeString(" ... ");
			write_generic_node(CASE_HIGH (node), nspaces, false);
		}
		else if (CASE_LOW (node))
		{
			writeString("case ");
			write_generic_node(CASE_LOW (node), nspaces,false);
		}
		else
			writeString("default");
		writeString(":");			
	}	

	//Handle goto expression
	if(TREE_CODE(node) == GOTO_EXPR)
	{
		int flag=0;
		op0 = GOTO_DESTINATION (node);
		if (TREE_CODE (op0) != SSA_NAME && DECL_P (op0) && DECL_NAME (op0))
		{
			const char *name = IDENTIFIER_POINTER (DECL_NAME (op0));
			if (strcmp (name, "break") == 0 || strcmp (name, "continue") == 0)
			{
				writeString(name);
				flag = 1;
			}
		}
		if(flag == 0)
		{
			writeString("goto ");
			write_generic_node(op0, nspaces, false);
			writeString(";");
		}
	}

	//Handle labels for goto and case
	if(TREE_CODE(node) == LABEL_EXPR)
	{
		op0 = TREE_OPERAND (node, 0);
		int flag = 0;
      		/* If this is for break or continue, don't bother printing it.  */
		if (DECL_NAME (op0))
		{
			const char *name = IDENTIFIER_POINTER (DECL_NAME (op0));
			if (strcmp (name, "break") == 0 || strcmp (name, "continue") == 0)
				flag = 1;
		}
		if(flag == 0)
		{

			write_generic_node(op0,nspaces,false);

			writeString(":");

			if (DECL_NONLOCAL (op0))
				writeString(" [non-local] ");
		}
	}

	//Handle label declaration for goto
	if(TREE_CODE(node) == LABEL_DECL)
	{
		if (DECL_NAME (node))			
			write_decl_name(node);
		else if (LABEL_DECL_UID (node) != -1)
			fprintf(fp, "L%d", (int) LABEL_DECL_UID (node));
		else
		{
			fprintf (fp, "D%u", DECL_UID (node));
		}
	}

	//Handle type declaraion for struct
	if(TREE_CODE(node) == TYPE_DECL)
	{
		int flag = 0;
		if (DECL_IS_BUILTIN (node))
		{
			flag = 1;
		}

		if(flag == 0)
		{
			if (DECL_NAME (node))
				write_decl_name(node);
			else if (TYPE_NAME (TREE_TYPE (node)) != node)
			{
				if ((TREE_CODE (TREE_TYPE (node)) == RECORD_TYPE || TREE_CODE (TREE_TYPE (node)) == UNION_TYPE)
					&& TYPE_METHODS (TREE_TYPE (node)))
				{
					//No classes so not needed
			      	}
				else
				{
					writeString((TREE_CODE (TREE_TYPE (node)) == UNION_TYPE ? "union" : "struct "));
					write_generic_node(TREE_TYPE (node), nspaces, false);
				}
			}
			else
				writeString("<anon>");
		}
	}


	return 0;
}

//Helper for printing declarations
void writeVarDeclOld(tree t,int nspaces)
{
	if (TREE_PUBLIC (t) && DECL_EXTERNAL (t))
		writeString("extern ");
	else if (TREE_STATIC (t))
		writeString("static ");

	
	write_generic_node(TREE_TYPE(t),0,false);
	writeSpace(1);

	
	fprintf(fp,"%s",IDENTIFIER_POINTER(DECL_NAME(t)));
	writeSpace(1);

	
	if (DECL_INITIAL(t))
	{
		writeString("= ");
		write_generic_node(DECL_INITIAL(t),0,false);
	}
}




//Helper for printing declarations
void writeVarDecl(tree t,int nspaces)
{
	INDENT (nspaces);
	
	if (TREE_CODE (t) == TYPE_DECL)
		writeString("typdef ");

	if (CODE_CONTAINS_STRUCT (TREE_CODE (t), TS_DECL_WRTL) && DECL_REGISTER (t))
		writeString("register ");

	if (TREE_PUBLIC (t) && DECL_EXTERNAL (t))
		writeString("extern ");
	else if (TREE_STATIC (t))
		writeString("static ");

	/* Print the type and name.  */
	if (TREE_TYPE (t) && TREE_CODE (TREE_TYPE (t)) == ARRAY_TYPE)
	{
		tree tmp;

		/* Print array's type.  */
		tmp = TREE_TYPE (t);
		while (TREE_CODE (TREE_TYPE (tmp)) == ARRAY_TYPE)
			tmp = TREE_TYPE (tmp);

		write_generic_node(TREE_TYPE (tmp), nspaces, false);

		/* Print variable's name.  */
		writeSpace(1);

		write_generic_node(t,nspaces,false);

		/* Print the dimensions.  */
		tmp = TREE_TYPE (t);
		while (TREE_CODE (tmp) == ARRAY_TYPE)
		{
			write_array_domain(TYPE_DOMAIN (tmp), nspaces);
			tmp = TREE_TYPE (tmp);
		}
	}
	else if (TREE_CODE (t) == FUNCTION_DECL)
	{
		write_generic_node(TREE_TYPE (TREE_TYPE (t)), nspaces , false);
		writeSpace(1);
		write_decl_name(t);
		write_function_declaration (TREE_TYPE (t), nspaces);
	}
	else
	{
		/* Print type declaration.  */
		write_generic_node(TREE_TYPE (t), nspaces, false);

		/* Print variable's name.  */
		writeSpace(1);
		write_generic_node(t, nspaces, false);
	}

	if (TREE_CODE (t) != FUNCTION_DECL)
	{
		/* Print the initial value.  */
		if (DECL_INITIAL (t))
		{
			writeSpace(1);
			writeString("=");
			writeSpace(1);
			write_generic_node(DECL_INITIAL (t), nspaces, false);
		}
	}
	writeString(";");
	
}

//Recusive function to print from reverse
void recurse(struct cgraph_node *node)
{
	if(node == NULL)
		return;

	recurse(node->next);	
	write_generic_node(node->decl,0,true);
}



void cs502_proj1()
{    

	struct varpool_node *nodeVar;	
 	fp = fopen("out.c","w");
	struct cgraph_node *node;
	struct cgraph_node *node_end;
	tree fndecl;
	
	  
	for (nodeVar = varpool_nodes; nodeVar; nodeVar = nodeVar->next) {
		writeNewLine();
		writeVarDecl(nodeVar->decl,0);
		writeString(";");
	} 


 	

	  printf ("*****************\n you may want add your code here\n **************\n");
	  
	  //handle the functions in a reverse order
	  recurse(cgraph_nodes);	 
	
	fclose(fp);
}



