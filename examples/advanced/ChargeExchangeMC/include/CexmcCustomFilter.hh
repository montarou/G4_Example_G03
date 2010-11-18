/*
 * =============================================================================
 *
 *       Filename:  CexmcCustomFilter.hh
 *
 *    Description:  custom filter grammar and compiler
 *
 *        Version:  1.0
 *        Created:  17.07.2010 15:31:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Alexey Radkov (), 
 *        Company:  PNPI
 *
 * =============================================================================
 */

#ifndef CEXMC_CUSTOM_FILTER_HH
#define CEXMC_CUSTOM_FILTER_HH

#ifdef CEXMC_USE_CUSTOM_FILTER

#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include "CexmcAST.hh"


namespace  CexmcCustomFilter
{
    using namespace boost::spirit;
    using namespace boost::spirit::qi;
    using namespace boost::spirit::ascii;
    using namespace boost::phoenix;
    using namespace CexmcAST;
    using boost::spirit::ascii::space;
    using boost::spirit::ascii::space_type;
    using boost::spirit::ascii::alpha;
    using boost::spirit::ascii::alnum;


    enum  Action
    {
        KeepTPT,
        KeepEDT,
        DeleteTPT,
        DeleteEDT
    };


    struct  ParseResult
    {
        ParseResult() : action( KeepTPT )
        {}

        void  Initialize( void )
        {
            action = KeepTPT;
            expression.children.clear();
            expression.type = Operator( Uninitialized );
        }

        Action   action;

        Subtree  expression;
    };


    struct  Compiler
    {
        template < typename  A, typename  B = boost::fusion::unused_type,
                   typename  C = boost::fusion::unused_type,
                   typename  D = boost::fusion::unused_type >
        struct  result { typedef void  type; };

        void  operator()( ParseResult &  parseResult, Action  value ) const;

        void  operator()( ParseResult &  parseResult, Subtree &  value ) const;

        void  operator()( Subtree &  ast, Node &  node ) const;

        void  operator()( Node &  self, Node &  left, Node &  right,
                          Operator  value ) const;

        void  operator()( Node &  self, Node &  child, Operator  value ) const;

        void  operator()( Node &  self, Node &  primary ) const;

        void  operator()( Node &  self, Node &  child, std::string &  value )
                                                                        const;

        void  operator()( Leaf &  self, std::string &  name ) const;

        void  operator()( Leaf &  self, int  value, size_t  index ) const;
    };


    template  < typename  Iterator >
    struct  Grammar : grammar< Iterator, ParseResult(), space_type >
    {
        Grammar();

        rule< Iterator, ParseResult(), space_type >            statement;

        rule< Iterator, Action(), space_type >                 action;

        rule< Iterator, Subtree(), space_type >                condition;

        rule< Iterator, Node(), space_type >                   expression;

        rule< Iterator, Node(), space_type >                   primary_expr;

        rule< Iterator, Node(), space_type >                   function1;

        rule< Iterator, std::string(), space_type >            identifier;

        rule< Iterator, Leaf(), space_type >                   leaf_operand;

        rule< Iterator, Leaf(), space_type >                   constant;

        rule< Iterator, Leaf(), space_type >                   variable;

        rule< Iterator, Node(), space_type >                   or_expr;

        rule< Iterator, Node(), space_type >                   and_expr;

        rule< Iterator, Node(), space_type >                   relation;

        rule< Iterator, Node(), space_type >                   addition;

        rule< Iterator, Node(), space_type >                   multiplication;

        rule< Iterator, Node(), space_type >                   unary_expr;

        rule< Iterator, Operator(), space_type >               unary_op;

        rule< Iterator, Operator(), space_type >               mult_op;

        rule< Iterator, Operator(), space_type >               add_op;

        rule< Iterator, Operator(), space_type >               rel_op;

        real_parser< double, strict_real_policies< double > >  strict_double;

        function< Compiler >                                   op;
    };


    template  < typename  Iterator >
    Grammar< Iterator >::Grammar() : Grammar::base_type( statement )
    {
        statement = action[ op( _val, _1 ) ] >>
                                        *( condition[ op( _val, _1 ) ] );

        action = lit( "keep" ) >>
                ( lit( "tpt" )[ _val = KeepTPT ] |
                  lit( "edt" )[ _val = KeepEDT ] ) |
                lit( "delete" ) >>
                ( lit ( "tpt" )[ _val = DeleteTPT ] |
                  lit ( "edt" )[ _val = DeleteEDT ] );

        condition = lit( "if" ) >> expression[ op( _val, _1 ) ];

        expression %= or_expr;

        identifier %= lexeme[ alpha >> *( alnum | lit( '_' ) ) ];

        primary_expr = function1[ _val = _1 ] |
                lit( '(' ) >> expression[ op( _val, _1 ) ] >> lit( ')' ) |
                leaf_operand[ _val = _1 ];

        leaf_operand %= constant | variable;

        constant %= strict_double | int_;

        variable = identifier[ op( _val, _1 ) ] >>
                -( lit( '[' ) >> ( uint_[ op( _val, _1, 0 ) ] - lit( '0' ) ) >>
                   -( lit( ',' ) >> ( uint_[ op( _val, _1, 1 ) ] -
                      lit( '0' ) ) ) >> lit( ']' ) );

        function1 = ( identifier >> lit( '(' ) >> expression >> lit( ')' ) )
                    [ op( _val, _2, _1 ) ];

        or_expr = ( and_expr >> lit( '|' ) >> or_expr )
                  [ op( _val, _1, _2, Operator( Or, 1 ) ) ] |
                  and_expr[ _val = _1 ];

        and_expr = ( relation >> lit( '&' ) >> and_expr )
                   [ op( _val, _1, _2, Operator( And, 2 ) ) ] |
                   relation[ _val = _1 ];

        relation = ( addition >> rel_op >> addition )
                   [ op( _val, _1, _3, _2 ) ] |
                   addition[ _val = _1 ];

        addition = ( multiplication >> add_op >> addition )
                   [ op( _val, _1, _3, _2 ) ] |
                   multiplication[ _val = _1 ];

        multiplication = ( unary_expr >> mult_op >> multiplication )
                         [ op( _val, _1, _3, _2 ) ] |
                         unary_expr[ _val = _1 ];

        unary_expr = ( unary_op >> primary_expr )[ op( _val, _2, _1 ) ] |
                     primary_expr[ _val = _1 ];

        unary_op = lit( '-' )[ _val = Operator( UMinus, 6, true ) ] |
                   lit( '!' )[ _val = Operator( Not, 6, true ) ];

        mult_op = lit( '*' )[ _val = Operator( Mult, 5 ) ] |
                  lit( '/' )[ _val = Operator( Div, 5 ) ];

        add_op = lit( '+' )[ _val = Operator( Plus, 4 ) ] |
                 lit( '-' )[ _val = Operator( Minus, 4 ) ];

        rel_op = lit( "<=" )[ _val = Operator( LessEq, 3 ) ] |
                 lit( ">=" )[ _val = Operator( MoreEq, 3 ) ] |
                 lit( "!=" )[ _val = Operator( NotEq, 3 ) ] |
                 lit( '<' )[ _val = Operator( Less, 3 ) ] |
                 lit( '>' )[ _val = Operator( More, 3 ) ] |
                 lit( '=' )[ _val = Operator( Eq, 3 ) ];
    }
}

#endif

#endif
