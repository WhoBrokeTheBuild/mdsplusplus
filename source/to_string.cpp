#include <mdsplus.hpp>

namespace mdsplus {

std::string to_string(const Usage& usage)
{
    switch (usage) {
    case Usage::Any: return "Usage::Any";
    case Usage::Structure: return "Usage::Structure";
    case Usage::Action: return "Usage::Action";
    case Usage::Device: return "Usage::Device";
    case Usage::Dispatch: return "Usage::Dispatch";
    case Usage::Numeric: return "Usage::Numeric";
    case Usage::Signal: return "Usage::Signal";
    case Usage::Task: return "Usage::Task";
    case Usage::Text: return "Usage::Text";
    case Usage::Window: return "Usage::Window";
    case Usage::Axis: return "Usage::Axis";
    case Usage::Subtree: return "Usage::Subtree";
    case Usage::CompoundData: return "Usage::CompoundData";
    }

    return "?";
}

std::string to_string(const Class& class_)
{
    switch (class_) {
        case Class::Missing: return"Class::Missing";
        case Class::S: return"Class::S";
        case Class::D: return"Class::D";
        case Class::A: return"Class::A";
        case Class::XD: return"Class::XD";
        case Class::XS: return"Class::XS";
        case Class::R: return"Class::R";
        case Class::CA: return"Class::CA";
        case Class::APD: return"Class::APD";
    }

    return "?";
}

std::string to_string(const DType& dtype)
{
    switch (dtype) {
        case DType::Missing: return "DType::Missing";
        case DType::BU: return "DType::BU";
        case DType::WU: return "DType::WU";
        case DType::LU: return "DType::LU";
        case DType::QU: return "DType::QU";
        case DType::B: return "DType::B";
        case DType::W: return "DType::W";
        case DType::L: return "DType::L";
        case DType::Q: return "DType::Q";
        case DType::F: return "DType::F";
        case DType::D: return "DType::D";
        case DType::FC: return "DType::FC";
        case DType::DC: return "DType::DC";
        case DType::T: return "DType::T";
        case DType::DSC: return "DType::DSC";
        case DType::OU: return "DType::OU";
        case DType::O: return "DType::O";
        case DType::G: return "DType::G";
        case DType::H: return "DType::H";
        case DType::GC: return "DType::GC";
        case DType::HC: return "DType::HC";

        case DType::Pointer: return "DType::Pointer";
        case DType::FS: return "DType::FS";
        case DType::FT: return "DType::FT";
        case DType::FSC: return "DType::FSC";
        case DType::FTC: return "DType::FTC";
        case DType::C: return "DType::C";

        case DType::List: return "DType::List";
        case DType::Tuple: return "DType::Tuple";
        case DType::Dictionary: return "DType::Dictionary";

        case DType::Ident: return "DType::Ident";
        case DType::NID: return "DType::NID";
        case DType::Path: return "DType::Path";
        case DType::Param: return "DType::Param";
        case DType::Signal: return "DType::Signal";
        case DType::Dimension: return "DType::Dimension";
        case DType::Window: return "DType::Window";
        case DType::Slope: return "DType::Slope";
        case DType::Function: return "DType::Function";
        case DType::Conglom: return "DType::Conglom";
        case DType::Range: return "DType::Range";
        case DType::Action: return "DType::Action";
        case DType::Dispatch: return "DType::Dispatch";
        case DType::Program: return "DType::Program";
        case DType::Routine: return "DType::Routine";
        case DType::Procedure: return "DType::Procedure";
        case DType::Method: return "DType::Method";
        case DType::Dependency: return "DType::Dependency";
        case DType::Condition: return "DType::Condition";
        case DType::Event: return "DType::Event";
        case DType::WithUnits: return "DType::WithUnits";
        case DType::Call: return "DType::Call";
        case DType::WithError: return "DType::WithError";
        case DType::Opaque: return "DType::Opaque";
    }

    return "?";
}

std::string to_string(const TreeNodeFlags& flags)
{
    std::string str;

    if (flags.CompressSegments) {
        str += "CompressSegments | ";
    }

    if (flags.IncludeInPulse) {
        str += "IncludeInPulse | ";
    }

    if (flags.NidReference) {
        str += "NidReference | ";
    }

    if (flags.PathReference) {
        str += "PathReference | ";
    }

    if (flags.NoWriteShot) {
        str += "NoWriteShot | ";
    }

    if (flags.NoWriteModel) {
        str += "NoWriteModel | ";
    }

    if (flags.CompressOnPut) {
        str += "CompressOnPut | ";
    }

    if (flags.DoNotCompress) {
        str += "DoNotCompress | ";
    }

    if (flags.Compressible) {
        str += "Compressible | ";
    }

    if (flags.WriteOnce) {
        str += "WriteOnce | ";
    }

    if (flags.SetupInformation) {
        str += "SetupInformation | ";
    }

    if (flags.Segmented) {
        str += "Segmented | ";
    }

    if (flags.Versions) {
        str += "Versions | ";
    }

    if (flags.Cached) {
        str += "Cached | ";
    }

    if (flags.Essential) {
        str += "Essential | ";
    }

    if (flags.ParentState) {
        str += "ParentState | ";
    }

    if (flags.State) {
        str += "State | ";
    }

    // Strip trailing " | "
    if (!str.empty()) {
        str.resize(str.size() - 3);
    }

    return str;
}

std::string to_string(const TreeNode * node)
{
    assert(node);

    std::string str = "TreeNode(";
    str += to_string(node->getTree());
    str += ", ";
    str += std::to_string(node->getNID());
    str += ")";

    return str;

    // return std::format(
    //     "TreeNode({}, {})",
    //     to_string(node->getTree()),
    //     node->getNID()
    // );
}

std::string to_string(const Mode& mode)
{
    switch (mode) {
    case Mode::Normal: return "Mode::Normal";
    case Mode::ReadOnly: return "Mode::ReadOnly";
    case Mode::Edit: return "Mode::Edit";
    case Mode::New: return "Mode::New";

    case Mode::View: return "Mode::View";
    }

    return "?";
}

std::string to_string(const Tree * tree)
{
    assert(tree);

    std::string str = "Tree(\"";
    str += tree->getTreeName();
    str += "\", ";
    str += std::to_string(tree->getShot());
    str += ", ";
    str += to_string(tree->getMode());
    str += ")";

    return str;

    // return std::format(
    //     "Tree(\"{}\", {}, {})",
    //     tree->getTreeName(),
    //     tree->getShot(),
    //     to_string(tree->getMode())
    // );
}

std::string to_string(const Data * data)
{
    assert(data);

    std::string str(data->getClassName());
    str += "(";
    str += to_string(data->getClass());
    str += ", ";
    str += to_string(data->getDType());
    str += ", `";
    str += data->decompile();
    str += "`)";

    return str;
}

} // namespace mdsplus