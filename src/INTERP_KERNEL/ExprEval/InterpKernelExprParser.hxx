#ifndef __INTERPKERNELEXPRPARSER_HXX__
#define __INTERPKERNELEXPRPARSER_HXX__

#include "INTERPKERNELEXPREVALDefines.hxx"
#include "InterpKernelUnit.hxx"
#include "InterpKernelException.hxx"
#include "InterpKernelFunction.hxx"

#include <string>
#include <list>
#include <map>
#include <set>

namespace INTERP_KERNEL
{
  class ValueDouble;

  class INTERPKERNELEXPREVAL_EXPORT LeafExpr
  {
  public:
    virtual ~LeafExpr();
    virtual void fillValue(Value *val) const throw(INTERP_KERNEL::Exception) = 0;
    static LeafExpr *buildInstanceFrom(const std::string& expr) throw(INTERP_KERNEL::Exception);
  };

  class INTERPKERNELEXPREVAL_EXPORT LeafExprVal : public LeafExpr
  {
  public:
    LeafExprVal(double value);
    ~LeafExprVal();
    void fillValue(Value *val) const throw(INTERP_KERNEL::Exception);
  private:
    double _value;
  };

  class INTERPKERNELEXPREVAL_EXPORT LeafExprVar : public LeafExpr
  {
  public:
    LeafExprVar(const std::string& var);
    ~LeafExprVar();
    void fillValue(Value *val) const throw(INTERP_KERNEL::Exception);
    std::string getVar() const { return _var_name; }
    void prepareExprEvaluation(const std::vector<std::string>& vars) const throw(INTERP_KERNEL::Exception);
    void prepareExprEvaluationVec() const throw(INTERP_KERNEL::Exception);
    static bool isRecognizedKeyVar(const std::string& var, int& pos);
  public:
    static const char END_OF_RECOGNIZED_VAR[];
  private:
    mutable int _fast_pos;
    std::string _var_name;
  };

  class INTERPKERNELEXPREVAL_EXPORT ExprParser
  {
  public:
    ExprParser(const char *expr);
    ExprParser(const char *expr, int lgth);
    ~ExprParser();
    void parse() throw(INTERP_KERNEL::Exception);
    bool isParsingSuccessfull() const { return _is_parsing_ok; }
    double evaluate() const throw(INTERP_KERNEL::Exception);
    DecompositionInUnitBase evaluateUnit() const throw(INTERP_KERNEL::Exception);
    void prepareExprEvaluation(const std::vector<std::string>& vars) const throw(INTERP_KERNEL::Exception);
    void evaluateExpr(int szOfOutParam, double *outParam, const double *inParam) const throw(INTERP_KERNEL::Exception);
    void prepareExprEvaluationVec() const throw(INTERP_KERNEL::Exception);
    void getSetOfVars(std::set<std::string>& vars) const;
    void getTrueSetOfVars(std::set<std::string>& vars) const;
    static std::string buildStringFromFortran(const char *expr, int lgth);
    static std::string deleteWhiteSpaces(const std::string& expr);
  private:
    Value *evaluateLowLev(Value *valGen) const throw(INTERP_KERNEL::Exception);
  private:
    void prepareExprEvaluationVecLowLev() const throw(INTERP_KERNEL::Exception);
    bool tryToInterpALeaf() throw(INTERP_KERNEL::Exception);
    void parseUnaryFunc() throw(INTERP_KERNEL::Exception);
    void parseForAddMin() throw(INTERP_KERNEL::Exception);
    void parseForMulDiv() throw(INTERP_KERNEL::Exception);
    void parseForPow() throw(INTERP_KERNEL::Exception);
    void parseDeeper() throw(INTERP_KERNEL::Exception);
    bool simplify() throw(INTERP_KERNEL::Exception);
    void releaseFunctions();
    void checkBracketsParity() const throw(INTERP_KERNEL::Exception);
    static std::size_t findCorrespondingOpenBracket(const std::string& expr, std::size_t posOfCloseBracket);
    static void locateError(std::ostream& stringToDisp, const std::string& srcOfErr, int posOfErr);
  private:
    bool _is_parsed;
    LeafExpr *_leaf;
    bool _is_parsing_ok;
    std::string _expr;
    std::list<ExprParser> _sub_expr;
    std::list<Function *> _func_btw_sub_expr;
  private:
    static const char WHITE_SPACES[];
    static const char EXPR_PARSE_ERR_MSG[];
  };
}

#endif
