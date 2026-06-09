using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using ItemEditor.Models.Schema;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Windows.Data;

namespace ItemEditor.ViewModels;

internal sealed partial class TraitsListViewModel : ObservableObject
{
    public ObservableCollection<TraitDefinition> AvailableTraits { get; } = [];
    public ICollectionView TraitsView { get; }

    [ObservableProperty]
    private string _searchText = string.Empty;

    public event EventHandler<TraitDefinition>? RequestAddTrait;

    public Func<TraitDefinition, bool>? CanAddTraitPredicate { get; set; }

    public TraitsListViewModel()
    {
        TraitsView = CollectionViewSource.GetDefaultView(AvailableTraits);
        TraitsView.Filter = FilterTraits;
    }

    partial void OnSearchTextChanged(string value) => TraitsView.Refresh();

    public void Initialize(IEnumerable<TraitDefinition> traits)
    {
        AvailableTraits.Clear();
        foreach (var trait in traits)
            AvailableTraits.Add(trait);
    }

    public void RefreshCommandStates() => AddTraitCommand.NotifyCanExecuteChanged();

    [RelayCommand(CanExecute = nameof(CanAddTrait))]
    private void AddTrait(object? parameter)
    {
        if (parameter is TraitDefinition trait)
            RequestAddTrait?.Invoke(this, trait);
    }

    private bool CanAddTrait(object? parameter)
    {
        if (parameter is not TraitDefinition trait) return false;
        return CanAddTraitPredicate?.Invoke(trait) ?? true;
    }

    private bool FilterTraits(object obj)
    {
        if (string.IsNullOrWhiteSpace(SearchText)) return true;
        if (obj is not TraitDefinition trait) return false;

        return trait.Id.Contains(SearchText, StringComparison.OrdinalIgnoreCase)
            || trait.DisplayName.Contains(SearchText, StringComparison.OrdinalIgnoreCase)
            || trait.Fields.Any(f => f.Name.Contains(SearchText, StringComparison.OrdinalIgnoreCase));
    }
}
