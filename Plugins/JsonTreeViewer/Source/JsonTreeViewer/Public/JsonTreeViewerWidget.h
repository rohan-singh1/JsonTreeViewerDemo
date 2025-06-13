//
// JSON Tree Viewer
// 
// Customizable slate widget exposed to Blueprints that visualizes a JSON file in a tree view 
//
// The MIT License (MIT)
//
// Copyright (c) 2025 Rohan Singh
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "JsonTreeViewerWidget.generated.h"

/**
 * UJsonTreeViewer
 *
 * A custom UWidget that parses JSON data and displays it as a collapsible tree view.
 */
UCLASS()
class JSONTREEVIEWER_API UJsonTreeViewerWidget : public UWidget
{
    GENERATED_BODY()

private:
    // Flag indicating whether the provided JSON string is valid
    bool _bValidJson;

    // Path to JSON file if reading from disk
    FString _JsonFilePath;

    // Raw JSON string content
    FString _JsonString;

    // Parsed JSON value root
    TSharedPtr<FJsonValue> _JsonValue;

    // Root Slate widget representing the JSON tree
    TSharedPtr<SWidget> _Widget;

    // Underlying STreeView widget to display tree items
    TSharedPtr<STreeView<TSharedPtr<class FTreeItem>>> _TreeView;

    // Top-level items in the tree
    TArray<TSharedPtr<class FTreeItem>> _TreeItems;

    // Generate a row widget for a given tree item
    TSharedRef<ITableRow> GenerateRow(TSharedPtr<class FTreeItem> Item, const TSharedRef<STableViewBase>& OwnerTable);

    // Retrieve children of a given tree item
    void GetChildren(TSharedPtr<class FTreeItem> Item, TArray<TSharedPtr<class FTreeItem>>& OutChildren);

    // Validate JSON string syntax
    bool IsValidJson(const FString& JsonString);

    // Parse raw JSON string into FJsonValue
    bool ParseJsonContents(FString JsonString, TSharedPtr<FJsonValue>& OutJsonValue);

    // Build tree items from parsed JSON
    void BuildTree(TSharedPtr<FJsonValue>& JsonValue);

    // Recursively parse a JSON node into a tree item
    TSharedPtr<FTreeItem> ParseNode(const TSharedPtr<FJsonValue>& JsonValue);

    // Map JSON value type to a Slate color for value text
    FSlateColor GetValueColorFromJsonType(EJson Type);

public:
    // Construct or rebuild the Slate widget for this UWidget
    virtual TSharedRef<SWidget> RebuildWidget() override;

    // Default constructor
    UJsonTreeViewerWidget();

    // Release Slate resources when the widget is destroyed
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;

    // Input JSON string or file path exposed to Blueprint
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "JSON Tree Viewer")
    FString JsonInput;

    // Color for JSON keys
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "JSON Tree Viewer")
    FSlateColor KeyColor;

    // Color for keys starting with '@'
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "JSON Tree Viewer")
    FSlateColor KeyAtColor;

    // Color for string values
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "JSON Tree Viewer")
    FSlateColor StringValueColor;

    // Color for boolean values
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "JSON Tree Viewer")
    FSlateColor BooleanValueColor;

    // Color for numeric values
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "JSON Tree Viewer")
    FSlateColor NumericValueColor;

    // Color for null values
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "JSON Tree Viewer")
    FSlateColor NullValueColor;

    // Padding applied to key, colon and value textboxes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "JSON Tree Viewer")
    FMargin Padding;

    // Custom font which can be set by the user
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "JSON Tree Viewer")
    FSlateFontInfo Font;

    // Read JSON file from disk into a string
    UFUNCTION(BlueprintCallable, Category = "JSON Tree Viewer")
    bool ReadJsonFile(const FString& FilePath, FString& JsonString);

    // Initialize the JSON tree from a raw JSON string or file path
    UFUNCTION(BlueprintCallable, Category = "JSON Tree Viewer")
    void InitJsonTree(const FString JsonPathOrString);
};


/**
 * FTreeItem
 *
 * Represents a node in the JSON tree
 */
class FTreeItem
{
public:
    FString Key;                                // JSON key for object members or array index
    FString Value;                              // String representation of the JSON value
    EJson ValueType;                            // Underlying JSON type
    TArray<TSharedPtr<FTreeItem>> ChildItems;   // Children of this node

    // Default constructor
    FTreeItem() : Key(TEXT("")), Value(TEXT("")), ValueType(EJson::None) {}

    // Constructor for a node with a specific JSON type
    FTreeItem(EJson Type) : Key(TEXT("")), Value(TEXT("")), ValueType(Type) {}

    // Constructor with key, value & JSON type
    FTreeItem(const FString& InKey, const FString& InValue, EJson InValueType)
        : Key(InKey), Value(InValue), ValueType(InValueType) {
    }
};
