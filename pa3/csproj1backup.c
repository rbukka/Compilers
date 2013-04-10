#include <stdio.h>
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "cgraph.h"
#include "hashtab.h"
#include "tree-iterator.h"
#include <stdio.h>
#define DEBUG 1

FILE *fp;  

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

void write_decl_name (tree node)
{
  if (DECL_NAME (node))
  {
	fprintf (fp,"%s",IDENTIFIER_POINTER(DECL_NAME (node)));
  } 
}

void write_function_name (tree node)
{
  if (TREE_CODE (node) == NOP_EXPR)
    node = TREE_OPERAND (node, 0);
  write_decl_name (node);
}

void write_call_name (tree node)
{
  tree op0 = node;

  if (TREE_CODE (op0) == NON_LVALUE_EXPR)
    op0 = TREE_OPERAND (op0, 0);

 again:
  switch (TREE_CODE (op0))
    {
    case VAR_DECL:
    case PARM_DECL:
    case FUNCTION_DECL:
      write_function_name (op0);
      break;

    case ADDR_EXPR:
    case INDIRECT_REF:
    case NOP_EXPR:
      op0 = TREE_OPERAND (op0, 0);
      goto again;

    case COND_EXPR:
      fprintf (fp,"%s", "(");
      write_generic_node (TREE_OPERAND (op0, 0));
      fprintf (fp, "%s",") ? ");
      write_generic_node (TREE_OPERAND (op0, 1));
      fprintf (fp,"%s", " : ");
      write_generic_node (TREE_OPERAND (op0, 2));
      break;

    case ARRAY_REF:
      if (TREE_CODE (TREE_OPERAND (op0, 0)) == VAR_DECL)
	write_function_name (TREE_OPERAND (op0, 0));
      else
	write_generic_node (op0);
      break;

    case MEM_REF:
      if (integer_zerop (TREE_OPERAND (op0, 1)))
	{
	  op0 = TREE_OPERAND (op0, 0);
	  goto again;
	}
      /* Fallthru.  */
    case COMPONENT_REF:
    case SSA_NAME:
    case OBJ_TYPE_REF:
      write_generic_node (op0);
      break;

    default:
   	printf("Not Handled");	
    }
}

int write_generic_node(tree node)
{
	tree parameters;
	tree blocks;
	tree variables;
	tree returnType;
	tree savedTree;
	tree savedTreeBody;
	tree_stmt_iterator si;
	tree statementList;
	tree op0,op1;

	if(TREE_CODE(node) == FUNCTION_DECL)
	{
		tree nodeTemp = node;
		
		savedTree = DECL_SAVED_TREE(nodeTemp);
		statementList = BIND_EXPR_BODY(savedTree);
	
		if(TREE_CODE(savedTree) == STATEMENT_LIST)
			printf("DECL_SAVED :\n");

		if(TREE_CODE(statementList) == STATEMENT_LIST)
			printf("DECL_SAVED STATEMENT :\n");



		//print return type
		returnType = DECL_RESULT(nodeTemp);
		check_type(TREE_TYPE(returnType));

		if(DEBUG)
			printf("Function name: %s %s\n",IDENTIFIER_POINTER(DECL_NAME(nodeTemp))," (");
		
		fprintf(fp,"%s%s",IDENTIFIER_POINTER(DECL_NAME(nodeTemp)),"(");

		char *comma= "";
		parameters = DECL_ARGUMENTS(nodeTemp);

		while(parameters)
		{
			fprintf(fp,"%s",comma);
			check_type(TREE_TYPE(parameters));
			fprintf(fp,"%s",IDENTIFIER_POINTER(DECL_NAME(parameters)));
			comma = ",";		
			if(DEBUG)
				printf("The decl name is %s\n",IDENTIFIER_POINTER(DECL_NAME(parameters)));
			parameters = TREE_CHAIN(parameters);
		}
		fprintf(fp,"%s",")");
		fprintf(fp,"%s\n","{");
		
		if(TREE_CODE(statementList) == STATEMENT_LIST)
		{
			for(si= tsi_start(statementList);!tsi_end_p(si);tsi_next(&si))
			{	
				tree stmt = tsi_stmt(si);
				if(stmt)
				{	
					switch(TREE_CODE(stmt))
					{
						case RETURN_EXPR:							
							fprintf(fp,"return ");
							op0 = TREE_OPERAND(stmt,0);

							if(op0)
							{
								fprintf(fp,"%s"," ");
								if(TREE_CODE(op0) == MODIFY_EXPR)
								{
									write_generic_node(TREE_OPERAND(op0,1));
								}
								else
								{
									write_generic_node(op0);
								}
							}
							break;
						case CALL_EXPR:
							printf("Call statement: \n");
							write_call_name (CALL_EXPR_FN (stmt));
							 /* Print parameters.  */
							 fprintf(fp,"%s"," ");
							 fprintf(fp, "%c",'(');
							 {
								tree arg;
								call_expr_arg_iterator iter;
								FOR_EACH_CALL_EXPR_ARG (arg, iter, stmt)
								  {
								    write_generic_node (arg);
								    if (more_call_expr_args_p (&iter))
								      {
									fprintf(fp,"%c", ',');
									fprintf(fp,"%s"," ");
								      }
								  }
							 }
							  if (CALL_EXPR_VA_ARG_PACK (stmt))
								{
								  if (call_expr_nargs (stmt) > 0)
								    {
								      fprintf(fp, "%c",',');
								      fprintf(fp,"%s"," ");
								    }
								  fprintf(fp,"%s","__builtin_va_arg_pack ()");
								}
								fprintf(fp,"%c",')');
								op1 = CALL_EXPR_STATIC_CHAIN (stmt);
								if (op1)
								{
									  fprintf(fp,"%s", " [static-chain: ");
									  write_generic_node (op1);
									  fprintf(fp,"%c", ']');
								}
	
						      if (CALL_EXPR_RETURN_SLOT_OPT (stmt))
							fprintf(fp,"%s"," [return slot optimization]");
						      if (CALL_EXPR_TAILCALL (stmt))
							fprintf(fp,"%s", " [tailcall]");								
							break;					
					}
				}
			}
		}
		
		if(TREE_CODE(statementList) == RETURN_EXPR)
		{
			fprintf(fp,"%s","return ");
			op0 = TREE_OPERAND(statementList,0);
			
			if(op0)
			{
				fprintf(fp,"%s"," ");
				if(TREE_CODE(op0) == MODIFY_EXPR)
				{
					write_generic_node(TREE_OPERAND(op0,1));
				}
				else
				{
					write_generic_node(op0);
				}
			}
			
		}	
	}	

	if(TREE_CODE(node) == CALL_EXPR)
	{
			printf("Call statement: \n");
			write_call_name (CALL_EXPR_FN (node));
			 /* Print parameters.  */
			 fprintf(fp,"%s"," ");
			 fprintf(fp, "%c",'(');
			 {
				tree arg;
				call_expr_arg_iterator iter;
				FOR_EACH_CALL_EXPR_ARG (arg, iter, node)
				{
					write_generic_node (arg);
					if (more_call_expr_args_p (&iter))
					{
						fprintf(fp,"%c", ',');
						fprintf(fp,"%s"," ");
			                }
				}
			 }
			 if (CALL_EXPR_VA_ARG_PACK (node))
			 {
				 if (call_expr_nargs (node) > 0)
				 {
					 fprintf(fp, "%c",',');
					 fprintf(fp,"%s"," ");
				 }
					  fprintf(fp,"%s","__builtin_va_arg_pack ()");
			}
			fprintf(fp,"%c",')');
			op1 = CALL_EXPR_STATIC_CHAIN (node);
			if (op1)
			{
				 fprintf(fp,"%s", " [static-chain: ");
				 write_generic_node (op1);
				 fprintf(fp,"%c", ']');
			}
		        if (CALL_EXPR_RETURN_SLOT_OPT (node))
				fprintf(fp,"%s"," [return slot optimization]");
			
			if (CALL_EXPR_TAILCALL (node))
				fprintf(fp,"%s", " [tailcall]");								

	}


	if(TREE_CODE(node) == MODIFY_EXPR || TREE_CODE(node) == INIT_EXPR)
	{
		write_generic_node (TREE_OPERAND (node, 0));
		fprintf (fp,"%s"," ");
		fprintf (fp,"%c",'=');
		if (TREE_CODE (node) == MODIFY_EXPR  && MOVE_NONTEMPORAL (node))
			fprintf (fp,"%s","{nt}");
		fprintf(fp,"%s"," ");
		write_generic_node(TREE_OPERAND (node, 1));      			
	}

	if(TREE_CODE(node) == NEGATE_EXPR || TREE_CODE(node) == BIT_NOT_EXPR || TREE_CODE(node) == TRUTH_NOT_EXPR || TREE_CODE(node) == ADDR_EXPR)
	{
		if (TREE_CODE (node) == ADDR_EXPR  && (TREE_CODE (TREE_OPERAND (node, 0)) == STRING_CST || TREE_CODE (TREE_OPERAND (node, 0)) ==  FUNCTION_DECL))
		;	
      		else
			fprintf (fp,"%s",op_symbol_code(TREE_CODE(node)));

		if (op_prio (TREE_OPERAND (node, 0)) < op_prio (node))
		{
			fprintf(fp,"%s",'(');
			write_generic_node(TREE_OPERAND (node, 0));
			fprintf(fp,"%s",')');
		}
		else
			write_generic_node(TREE_OPERAND (node, 0));    			
	}

	if(TREE_CODE(node) == TRUTH_OR_EXPR || TREE_CODE(node) == TRUTH_ORIF_EXPR || TREE_CODE(node) == TRUTH_AND_EXPR)
	{
		const char *op = op_symbol_code(TREE_CODE(node));
		op0 = TREE_OPERAND (node, 0);
		op1 = TREE_OPERAND (node, 1);

		/* When the operands are expressions with less priority,
		   keep semantics of the tree representation.  */
		if (op_prio (op0) <= op_prio (node))
		  {
		    fprintf(fp,"%c", '(');
		    write_generic_node(op0);
		    fprintf(fp,"%c", ')');
		  }
		else
		  write_generic_node(op0);

		fprintf(fp,"%s"," ");
		fprintf(fp,"%s",op);
		fprintf(fp,"%s"," ");

		/* When the operands are expressions with less priority,
		   keep semantics of the tree representation.  */
		if (op_prio (op1) <= op_prio (node))
		  {
		    fprintf(fp,"%c",'(');
		    write_generic_node(op1);
		    fprintf(fp,"%c", ')');
		  }
		else
		  write_generic_node(op1);
	}
	
	if(TREE_CODE(node) == TRUTH_ANDIF_EXPR || TREE_CODE(node) == MULT_EXPR || TREE_CODE(node) == PLUS_EXPR || TREE_CODE(node) == MINUS_EXPR)
	{
		const char *op = op_symbol_code(TREE_CODE(node));
		op0 = TREE_OPERAND (node, 0);
		op1 = TREE_OPERAND (node, 1);

		/* When the operands are expressions with less priority,
		   keep semantics of the tree representation.  */
		if (op_prio (op0) <= op_prio (node))
		  {
		    fprintf(fp,"%c", '(');
		    write_generic_node(op0);
		    fprintf(fp,"%c", ')');
		  }
		else
		  write_generic_node(op0);

		fprintf(fp,"%s"," ");
		fprintf(fp,"%s",op);
		fprintf(fp,"%s"," ");

		/* When the operands are expressions with less priority,
		   keep semantics of the tree representation.  */
		if (op_prio (op1) <= op_prio (node))
		  {
		    fprintf(fp,"%c",'(');
		    write_generic_node(op1);
		    fprintf(fp,"%c", ')');
		  }
		else
		  write_generic_node(op1);
	}

	if(TREE_CODE(node) == TRUNC_DIV_EXPR || TREE_CODE(node) == CEIL_DIV_EXPR || TREE_CODE(node) == FLOOR_DIV_EXPR || TREE_CODE(node) == ROUND_DIV_EXPR)
	{
		const char *op = op_symbol_code(TREE_CODE(node));
		op0 = TREE_OPERAND (node, 0);
		op1 = TREE_OPERAND (node, 1);

		/* When the operands are expressions with less priority,
		   keep semantics of the tree representation.  */
		if (op_prio (op0) <= op_prio (node))
		  {
		    fprintf(fp,"%c", '(');
		    write_generic_node(op0);
		    fprintf(fp,"%c", ')');
		  }
		else
		  write_generic_node(op0);

		fprintf(fp,"%s"," ");
		fprintf(fp,"%s",op);
		fprintf(fp,"%s"," ");

		/* When the operands are expressions with less priority,
		   keep semantics of the tree representation.  */
		if (op_prio (op1) <= op_prio (node))
		  {
		    fprintf(fp,"%c",'(');
		    write_generic_node(op1);
		    fprintf(fp,"%c", ')');
		  }
		else
		  write_generic_node(op1);
	}
	
	if(TREE_CODE(node) == RDIV_EXPR || TREE_CODE(node) == EXACT_DIV_EXPR || TREE_CODE(node) == LT_EXPR || TREE_CODE(node) == LE_EXPR)
	{
		const char *op = op_symbol_code(TREE_CODE(node));
		op0 = TREE_OPERAND (node, 0);
		op1 = TREE_OPERAND (node, 1);

		/* When the operands are expressions with less priority,
		   keep semantics of the tree representation.  */
		if (op_prio (op0) <= op_prio (node))
		  {
		    fprintf(fp,"%c", '(');
		    write_generic_node(op0);
		    fprintf(fp,"%c", ')');
		  }
		else
		  write_generic_node(op0);

		fprintf(fp,"%s"," ");
		fprintf(fp,"%s",op);
		fprintf(fp,"%s"," ");

		/* When the operands are expressions with less priority,
		   keep semantics of the tree representation.  */
		if (op_prio (op1) <= op_prio (node))
		  {
		    fprintf(fp,"%c",'(');
		    write_generic_node(op1);
		    fprintf(fp,"%c", ')');
		  }
		else
		  write_generic_node(op1);
	}

	if(TREE_CODE(node) == GT_EXPR || TREE_CODE(node) == GE_EXPR || TREE_CODE(node) == EQ_EXPR || TREE_CODE(node) == NE_EXPR)
	{
		const char *op = op_symbol_code(TREE_CODE(node));
		op0 = TREE_OPERAND (node, 0);
		op1 = TREE_OPERAND (node, 1);

		/* When the operands are expressions with less priority,
		   keep semantics of the tree representation.  */
		if (op_prio (op0) <= op_prio (node))
		  {
		    fprintf(fp,"%c", '(');
		    write_generic_node(op0);
		    fprintf(fp,"%c", ')');
		  }
		else
		  write_generic_node(op0);

		fprintf(fp,"%s"," ");
		fprintf(fp,"%s",op);
		fprintf(fp,"%s"," ");

		/* When the operands are expressions with less priority,
		   keep semantics of the tree representation.  */
		if (op_prio (op1) <= op_prio (node))
		  {
		    fprintf(fp,"%c",'(');
		    write_generic_node(op1);
		    fprintf(fp,"%c", ')');
		  }
		else
		  write_generic_node(op1);
	}

	if(TREE_CODE(node) == PARM_DECL)
	{
		fprintf(fp,"%s",IDENTIFIER_POINTER(DECL_NAME(node)));
	}
	
	return 0;
}

void cs502_proj1()
{  
 	  struct cgraph_node *node;
	  struct cgraph_node *node_end;
	  tree fndecl;
	  fp = fopen("out.c","w");

	  printf ("*****************\n you may want add your code here\n **************\n");
	  //traverse all functions
	  for (node = cgraph_nodes; node; node = node->next) {
	    	fndecl = node->decl;
       		//TODO : unparsing a function
		if(DEBUG)
		{
			write_generic_node(fndecl);			
		}
		else
		{
			printf("Not a function Node\n");
		}
		fprintf(fp,"%s\n","}");
	}
	
	fclose(fp);
}



