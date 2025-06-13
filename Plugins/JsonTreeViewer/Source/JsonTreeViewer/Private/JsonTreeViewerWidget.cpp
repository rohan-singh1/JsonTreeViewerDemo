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

#include "JsonTreeViewerWidget.h"
#include "Serialization/JsonSerializer.h" 
#include "Dom/JsonObject.h" 
#include "Logging/LogMacros.h" 
#include "Styling/CoreStyle.h"

UJsonTreeViewerWidget::UJsonTreeViewerWidget()
{
    // Set up default colors for different JSON value types
    NumericValueColor = FLinearColor(0.843f, 0.591281f, 0.073341f);     // Dark Yellow for numbers
    StringValueColor = FLinearColor(0.839f, 0.349094f, 0.125011f);      // Buff for strings
    BooleanValueColor = FLinearColor(0.839f, 0.251924f, 0.582957);      // Pink for booleans
    NullValueColor = FLinearColor(1.0f, 0.3f, 0.3f);                    // Red for null values
    KeyColor = FLinearColor(0.370628f, 0.808f, 0.143016f);              // Pale Green for keys
    KeyAtColor = FLinearColor(0.5f, 0.5f, 1.f);                         // Mauve for keys starting with '@'
    Padding = FMargin(2.f);                                             // Uniform padding around elements
}

TSharedRef<SWidget> UJsonTreeViewerWidget::RebuildWidget()
{
    InitJsonTree(JsonInput);  // Parse the JSON string or file into a tree structure
    _Widget = SNew(SBox)
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .FillHeight(1.f)
                [
                    SNew(SScrollBox)
                        + SScrollBox::Slot()
                        [
                            SAssignNew(_TreeView, STreeView<TSharedPtr<class FTreeItem>>)
                                .TreeItemsSource(&_TreeItems)
                                .SelectionMode(ESelectionMode::None)
                                .OnGenerateRow_UObject(this, &UJsonTreeViewerWidget::GenerateRow)
                                .OnGetChildren_UObject(this, &UJsonTreeViewerWidget::GetChildren)
                        ]
                ]
        ];

    return _Widget.ToSharedRef();
}

void UJsonTreeViewerWidget::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);

    // Release the widget from memory
    _Widget.Reset();
}

bool UJsonTreeViewerWidget::ReadJsonFile(const FString& FilePath, FString& JsonString)
{
    // Read the entire file contents into JsonString; warn if it fails
    if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to read the file: %s"), *FilePath);
        return false;
    }
    return true;
}

void UJsonTreeViewerWidget::InitJsonTree(const FString JsonPathorString)
{
    // Determine if the input is a file path or raw JSON string
    if (FPaths::FileExists(JsonPathorString))
    {
        JsonInput = JsonPathorString;
        _JsonFilePath = JsonPathorString;
        _bValidJson = ReadJsonFile(_JsonFilePath, _JsonString);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("This does not look like a valid file path: %s\nChecking if it is a JSON string..."), *JsonPathorString);

        // Check for valid JSON text
        if (IsValidJson(JsonPathorString))
        {
            JsonInput = JsonPathorString;
            _JsonString = JsonPathorString;
            _bValidJson = true;
            UE_LOG(LogTemp, Log, TEXT("This appears to be a valid JSON string..."));
        }
        else
        {
            _bValidJson = false;
            UE_LOG(LogTemp, Warning, TEXT("This string '%s' is neither a valid file path nor a valid JSON string! Aborting!"), *JsonPathorString);
        }
    }

    // Parse and build the tree if the JSON is valid
    if (_bValidJson)
    {
        if (ParseJsonContents(_JsonString, _JsonValue) && _JsonValue)
        {
            BuildTree(_JsonValue);
        }
    }
}

TSharedRef<ITableRow> UJsonTreeViewerWidget::GenerateRow(TSharedPtr<class FTreeItem> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
    // Use default Slate texboxt font if no custom font is provided
    const FSlateFontInfo DefaultFont(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 9);

    // Create the treeview row widget
    return SNew(STableRow<TSharedPtr<class FTreeItem>>, OwnerTable)
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .Padding(Padding)
                .AutoWidth()
                [
                    SNew(SEditableText)
                        .IsReadOnly(true)
                        .Visibility(Item->Key.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible)
                        .Text(FText::FromString(Item->Key))
                        .ColorAndOpacity(Item->Key.Left(1) == TEXT("@") ? KeyAtColor : KeyColor) // '@' keys get special color
                        .Font(!Font.FontObject ? DefaultFont : Font)
                ]
                + SHorizontalBox::Slot()
                .Padding(Padding)
                .AutoWidth()
                [
                    SNew(STextBlock)
                        .Visibility(Item->Key.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible)
                        .Text(FText::FromString(":"))
                        .Font(!Font.FontObject ? DefaultFont : Font)
                ]
                + SHorizontalBox::Slot()
                .Padding(Padding)
                .AutoWidth()
                [
                    SNew(SEditableText)
                        .IsReadOnly(true)
                        .Visibility(Item->Value.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible)
                        .Text(FText::FromString(Item->Value))
                        .ColorAndOpacity(GetValueColorFromJsonType(Item->ValueType)) // Color based on value type
                        .Font(!Font.FontObject ? DefaultFont : Font)
                ]
        ];
}

void UJsonTreeViewerWidget::GetChildren(TSharedPtr<class FTreeItem> Item, TArray<TSharedPtr<class FTreeItem>>& OutChildren)
{
    // Provide the child items for a given tree node
    OutChildren = Item->ChildItems;
}

bool UJsonTreeViewerWidget::IsValidJson(const FString& JsonString)
{
    // Attempt to deserialize into a JSON object to check validity
    TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
    TSharedPtr<FJsonObject> JsonObject;

    return FJsonSerializer::Deserialize(JsonReader, JsonObject);
}

bool UJsonTreeViewerWidget::ParseJsonContents(FString JsonString, TSharedPtr<FJsonValue>& OutJsonValue)
{
    // If valid JSON, deserialize into a JsonValue container
    if (IsValidJson(JsonString))
    {
        TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
        return FJsonSerializer::Deserialize(JsonReader, OutJsonValue);
    }
    return false;
}

void UJsonTreeViewerWidget::BuildTree(TSharedPtr<FJsonValue>& JsonValue)
{
    _TreeItems.Reset();  // Clear any existing tree

    // Handle array at root
    if (JsonValue->Type == EJson::Array)
    {
        for (auto& Element : JsonValue->AsArray())
        {
            _TreeItems.Add(ParseNode(Element));
        }
    }
    // Handle object at root
    else if (JsonValue->Type == EJson::Object)
    {
        for (auto& Pair : JsonValue->AsObject()->Values)
        {
            TSharedPtr<FTreeItem> Parsed = ParseNode(Pair.Value);
            TSharedPtr<FTreeItem> Row = MakeShared<FTreeItem>(Pair.Key, Parsed->Value, Parsed->ValueType);
            if (Parsed->ChildItems.Num() > 0)
            {
                Row->ChildItems = Parsed->ChildItems;
            }
            _TreeItems.Add(Row);
        }
    }
    // Handle single primitive at root
    else
    {
        _TreeItems.Add(ParseNode(JsonValue));
    }
}

TSharedPtr<FTreeItem> UJsonTreeViewerWidget::ParseNode(const TSharedPtr<FJsonValue>& JsonValue)
{
    // Create a new tree item for this JSON value
    TSharedPtr<FTreeItem> Node = MakeShared<FTreeItem>(JsonValue->Type);

    switch (JsonValue->Type)
    {
    case EJson::Object:
        // Recurse into each member of the object
        for (auto& Pair : JsonValue->AsObject()->Values)
        {
            TSharedPtr<FTreeItem> Parsed = ParseNode(Pair.Value);
            TSharedPtr<FTreeItem> Child = MakeShared<FTreeItem>(Pair.Key, Parsed->Value, Parsed->ValueType);
            if (Parsed->ChildItems.Num() > 0)
            {
                Child->ChildItems = Parsed->ChildItems;
            }
            Node->ChildItems.Add(Child);
        }
        break;

    case EJson::Array:
        // Flatten array elements under this node
        for (auto& Element : JsonValue->AsArray())
        {
            TSharedPtr<FTreeItem> Parsed = ParseNode(Element);
            if (Parsed->ChildItems.Num() > 0)
            {
                Node->ChildItems.Append(Parsed->ChildItems);
            }
            else
            {
                Node->ChildItems.Add(Parsed);
            }
        }
        break;

    case EJson::String:
    {
        // Extract string value
        JsonValue->TryGetString(Node->Value);
        Node->ValueType = EJson::String;
        break;
    }

    case EJson::Number:
    {
        // Extract numeric value and sanitize to string
        double NumberValue = 0;
        JsonValue->TryGetNumber(NumberValue);
        Node->Value = FString::SanitizeFloat(NumberValue);
        Node->ValueType = EJson::Number;
        break;
    }

    case EJson::Boolean:
    {
        // Extract boolean value and convert to text
        bool BoolValue = false;
        JsonValue->TryGetBool(BoolValue);
        Node->Value = BoolValue ? TEXT("true") : TEXT("false");
        Node->ValueType = EJson::Boolean;
        break;
    }

    case EJson::Null:
    {
        // Display null in case of null data
        Node->Value = TEXT("null");
        Node->ValueType = EJson::Null;
        break;
    }

    default:
        // Ignore the Nothing type data
        break;
    }

    return Node;
}

FSlateColor UJsonTreeViewerWidget::GetValueColorFromJsonType(EJson Type)
{
    // Return the matching color for each JSON type,
    // else return the default Slate textblock color
    switch (Type)
    {
    case EJson::String:  return StringValueColor;
    case EJson::Number:  return NumericValueColor;
    case EJson::Boolean: return BooleanValueColor;
    case EJson::Null:    return NullValueColor;
    default:             return FSlateColor::UseForeground();
    }
}
