import sympy
import numpy as np
import matplotlib.pyplot as plt

def extract_coefficients(expr) -> np.array:
	"""Yeah, yeah, there is a faster way than using the string representation...idc"""
	s = str(expr)
	summands = s.split("+")
	coeffs = np.zeros((childhood+1,), dtype=np.int64)
	for summ in summands:
		data = summ.split("p0_")
		try:
			k = int(data[0][:-1])
		except ValueError:
			k = 1
		age = int(data[1])
		coeffs[age] = k
	return coeffs

repr_cycle = 6
childhood = 8

def solve(n_days, print_expr = False, generate_coeffs = False):
	population = np.array([sympy.Symbol(f"p0_{j}", integer=True) for j in range(childhood+1)])

	day = 0
	if print_expr:
		print(f"p({day+1:>3}) = {sum(population)}")

	if generate_coeffs:
		coeffs = np.zeros((childhood+1, n_days+1), dtype=np.int64)
		coeffs[:,0]	= extract_coefficients(sum(population))

	for day in range(n_days):
		
		new_fishies = population[0]
		
		for i in range(childhood):
			population[i] = population[i+1]

		population[repr_cycle] += new_fishies
		population[childhood] = new_fishies

		if print_expr:
			print(f"p({day+1:>3}) = {sum(population)}")
		if generate_coeffs:
			coeffs[:,day+1]	= extract_coefficients(sum(population))
	if generate_coeffs:
		return (sum(population), coeffs)
	return sum(population)

# TEST
data = [3,4,3,1,2]
test_data = np.zeros((childhood+1,), dtype=np.int64)
for d in data:
	test_data[int(d)] += 1

solution = solve(256)
coeff = extract_coefficients(solution)

print("Test:")
print("Obtained:", (test_data @ coeff))
print("Expected:", 26984457539)
print("---")

# Get polynomial
print("Analytical expression:")
print("p(256) = ", solution)
print("---")


# Plot function
days = 80
_, coeffs = solve(days, print_expr=False, generate_coeffs=True)

X = np.array([i for i in range(days)])

for age in range(childhood+1):
	Y = [coeffs[age, d] for d in range(days)]
	plt.semilogy(X, Y, label=f"k_{age}")

plt.legend()
plt.title("population(day) = inner_prod(initial_populations, k)")
plt.xlabel("Day")
plt.grid()
plt.show()