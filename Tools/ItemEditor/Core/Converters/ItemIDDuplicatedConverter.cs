using ItemEditor.Services;
using System.Globalization;
using System.Windows.Data;

namespace ItemEditor.Core.Converters;

internal sealed class ItemIDDuplicatedConverter : IMultiValueConverter
{
    public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
    {
        if (values.Length >= 2 && values[0] is string id && values[1] is ItemIDRegistryService registry)
            return registry.IsIDDuplicate(id);
        return false;
    }

    public object[] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
    {
        throw new NotImplementedException();
    }
}
