#pragma once

#include <Columns/IColumn.h>
#include <Columns/ColumnVector.h>
#include <DataStreams/IProfilingBlockInputStream.h>
#include <DataTypes/DataTypesNumber.h>
#include <DataTypes/DataTypeString.h>
#include <DataTypes/DataTypeDate.h>
// TODO: refine includes
#include <arrow/api.h>

namespace DB
{

class ParquetBlockInputStream : public IProfilingBlockInputStream
{
public:
    ParquetBlockInputStream(ReadBuffer & istr_, const Block & header_);

    String getName() const override { return "Parquet"; }
    Block getHeader() const override;

protected:
    Block readImpl() override;

private:
    ReadBuffer & istr;
    Block header;

    static void fillColumnWithStringData(std::shared_ptr<arrow::Column> & arrow_column, MutableColumnPtr & internal_column);
    static void fillColumnWithBooleanData(std::shared_ptr<arrow::Column> & arrow_column, MutableColumnPtr & internal_column);
    static void fillColumnWithDate32Data(std::shared_ptr<arrow::Column> & arrow_column, MutableColumnPtr & internal_column);
    template <typename NumericType>
    static void fillColumnWithNumericData(std::shared_ptr<arrow::Column> & arrow_column, MutableColumnPtr & internal_column);

    static void fillByteMapFromArrowColumn(std::shared_ptr<arrow::Column> & arrow_column, MutableColumnPtr & bytemap);

    static const std::unordered_map<arrow::Type::type, std::shared_ptr<IDataType>> arrow_type_to_internal_type;

    // TODO: check that this class implements every part of its parent
};

}